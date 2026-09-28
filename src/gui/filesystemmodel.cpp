#include "filesystemmodel.h"
#include <KOpenWithDialog>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QStorageInfo>

void FileSystemModel::scanDesktopApps()
{
    m_systemAppsCache.clear();

    QStringList searchDirs = {QStringLiteral("/usr/share/applications"),
                              QStringLiteral("/usr/local/share/applications"),
                              QDir::homePath() + QStringLiteral("/.local/share/applications"),
                              QStringLiteral("/var/lib/flatpak/exports/share/applications"),
                              QDir::homePath() + QStringLiteral("/.local/share/flatpak/exports/share/applications")};

    QSet<QString> seenKeys;

    for (const QString &dirPath : searchDirs) {
        QDir dir(dirPath);
        if (!dir.exists())
            continue;

        const QStringList entries = dir.entryList(QStringList() << QStringLiteral("*.desktop"), QDir::Files);
        for (const QString &fileName : entries) {
            QString fullPath = dir.absoluteFilePath(fileName);
            QSettings desktop(fullPath, QSettings::IniFormat);
            desktop.beginGroup(QStringLiteral("Desktop Entry"));

            QString type = desktop.value(QStringLiteral("Type")).toString();
            bool noDisplay = desktop.value(QStringLiteral("NoDisplay"), false).toBool();
            if (type != QLatin1String("Application") || noDisplay) {
                desktop.endGroup();
                continue;
            }

            QString name = desktop.value(QStringLiteral("Name[es]")).toString();
            if (name.isEmpty())
                name = desktop.value(QStringLiteral("Name[es_ES]")).toString();
            if (name.isEmpty())
                name = desktop.value(QStringLiteral("Name")).toString();

            QString execStr = desktop.value(QStringLiteral("Exec")).toString();
            QString iconStr = desktop.value(QStringLiteral("Icon")).toString();
            QString mimeTypesStr = desktop.value(QStringLiteral("MimeType")).toString();

            desktop.endGroup();

            if (name.isEmpty() || execStr.isEmpty())
                continue;

            QString cleanCmd = execStr;
            cleanCmd.remove(QStringLiteral("%f"))
                .remove(QStringLiteral("%F"))
                .remove(QStringLiteral("%u"))
                .remove(QStringLiteral("%U"))
                .remove(QStringLiteral("%i"))
                .remove(QStringLiteral("%c"))
                .remove(QStringLiteral("%k"));
            cleanCmd = cleanCmd.trimmed();

            if (cleanCmd.isEmpty())
                continue;

            QString key = name.toLower() + QLatin1Char('|') + cleanCmd.toLower();
            if (seenKeys.contains(key))
                continue;
            seenKeys.insert(key);

            SystemAppInfo info;
            info.name = name;
            info.cmd = cleanCmd;
            info.icon = iconStr;
            if (!mimeTypesStr.isEmpty()) {
                const QStringList parts = mimeTypesStr.split(QLatin1Char(';'), Qt::SkipEmptyParts);
                for (const QString &p : parts) {
                    info.mimeTypes.append(p.trimmed().toLower());
                }
            }
            m_systemAppsCache.append(info);
        }
    }
}

QVariantList FileSystemModel::getOpenWithApps(const QString &path)
{
    QVariantList result;
    QFileInfo fi(path);
    if (!fi.exists())
        return result;

    if (m_systemAppsCache.isEmpty()) {
        scanDesktopApps();
    }

    QMimeDatabase mimeDb;
    QMimeType fileMime = mimeDb.mimeTypeForFile(path);
    QString mimeName = fileMime.name().toLower();
    QString ext = fi.suffix().toLower();

    struct AppItem {
        QString name;
        QString cmd;
        QString icon;
        bool isRecommended;
    };
    QList<AppItem> appList;
    appList.reserve(m_systemAppsCache.size());

    for (const SystemAppInfo &app : m_systemAppsCache) {
        bool isRecommended = false;
        for (const QString &m : app.mimeTypes) {
            if ((!mimeName.isEmpty() && m == mimeName) || (!ext.isEmpty() && m.contains(ext))) {
                isRecommended = true;
                break;
            }
        }

        // Browser matching helper
        if (ext == QLatin1String("html") || ext == QLatin1String("htm") || ext == QLatin1String("url") || mimeName.contains(QStringLiteral("html"))) {
            if (app.cmd.contains(QStringLiteral("chrome")) || app.cmd.contains(QStringLiteral("firefox")) || app.cmd.contains(QStringLiteral("edge"))
                || app.cmd.contains(QStringLiteral("brave")) || app.cmd.contains(QStringLiteral("vivaldi")) || app.cmd.contains(QStringLiteral("opera"))) {
                isRecommended = true;
            }
        }

        AppItem item;
        item.name = app.name;
        item.cmd = app.cmd;
        item.icon = app.icon;
        item.isRecommended = isRecommended;
        appList.append(item);
    }

    std::sort(appList.begin(), appList.end(), [](const AppItem &a, const AppItem &b) {
        if (a.isRecommended != b.isRecommended) {
            return a.isRecommended > b.isRecommended;
        }
        return a.name.localeAwareCompare(b.name) < 0;
    });

    for (const AppItem &app : appList) {
        QVariantMap map;
        map[QStringLiteral("name")] = app.name;
        map[QStringLiteral("cmd")] = app.cmd;
        map[QStringLiteral("icon")] = app.icon;
        map[QStringLiteral("isRecommended")] = app.isRecommended;
        result.append(map);
    }

    return result;
}

void FileSystemModel::launchWithApp(const QString &path, const QString &execCmd)
{
    qDebug() << "[GFaster] Launching path:" << path << "with app cmd:" << execCmd;
    if (path.isEmpty() || execCmd.isEmpty())
        return;

    QString cleanCmd = execCmd;
    cleanCmd.remove(QStringLiteral("%f"))
        .remove(QStringLiteral("%F"))
        .remove(QStringLiteral("%u"))
        .remove(QStringLiteral("%U"))
        .remove(QStringLiteral("%i"))
        .remove(QStringLiteral("%c"))
        .remove(QStringLiteral("%k"));
    cleanCmd = cleanCmd.trimmed();

    QStringList parts = QProcess::splitCommand(cleanCmd);
    if (!parts.isEmpty()) {
        QString program = parts.takeFirst();
        parts.append(path);
        QProcess::startDetached(program, parts);
    } else {
        QProcess::startDetached(cleanCmd, QStringList() << path);
    }
}

void FileSystemModel::openWithSystemDialog(const QString &path)
{
    qDebug() << "[GFaster] Launching native KDE open-with dialog for:" << path;
    if (path.isEmpty())
        return;

    QUrl url = QUrl::fromLocalFile(path);
    KOpenWithDialog *dialog = new KOpenWithDialog(QList<QUrl>{url}, QString(), QString(), nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

FileSystemModel::FileSystemModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_searchProcess(new QProcess(this))
    , m_searchTimer(new QTimer(this))
{
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, &QTimer::timeout, this, &FileSystemModel::performSearch);
    connect(m_searchProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &FileSystemModel::onSearchProcessFinished);

    scanDesktopApps();

    QString homePath = QDir::homePath();
    loadDirectory(homePath);
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return QVariant();

    const FileItem &item = m_items.at(index.row());

    switch (role) {
    case NameRole:
        return item.name;
    case PathRole:
        return item.path;
    case SizeRole:
        return item.sizeStr;
    case TypeRole:
        return item.typeStr;
    case IsDirRole:
        return item.isDir;
    case IconNameRole:
        return item.iconName;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FileSystemModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    roles[SizeRole] = "sizeStr";
    roles[TypeRole] = "typeStr";
    roles[IsDirRole] = "isDir";
    roles[IconNameRole] = "iconName";
    return roles;
}

void FileSystemModel::setCurrentPath(const QString &path)
{
    if (m_currentPath != path) {
        loadDirectory(path);
    }
}

void FileSystemModel::openDir(const QString &path)
{
    loadDirectory(path);
}

void FileSystemModel::openParentDir()
{
    QDir dir(m_currentPath);
    if (dir.cdUp()) {
        loadDirectory(dir.absolutePath());
    }
}

void FileSystemModel::openFile(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists()) {
        qWarning() << "[GFaster] File does not exist:" << path;
        return;
    }

    qDebug() << "[GFaster] Launching file via xdg-open:" << path;

    QString ext = fi.suffix().toLower();

    // Direct binary/script execution only for explicit executable extensions
    if ((ext == QLatin1String("sh") || ext == QLatin1String("appimage") || ext == QLatin1String("run") || ext == QLatin1String("bin")) && fi.isExecutable()) {
        bool started = QProcess::startDetached(path, QStringList());
        qDebug() << "[GFaster] Executed script/binary:" << path << "Result:" << started;
        if (started)
            return;
    }

    // Default system application launcher (xdg-open) for videos, images, audio, docs, etc.
    bool ok = QProcess::startDetached(QStringLiteral("xdg-open"), QStringList() << path);
    qDebug() << "[GFaster] Launched xdg-open for:" << path << "Result:" << ok;

    if (!ok) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void FileSystemModel::openItem(const QString &path, bool isDir)
{
    qDebug() << "[GFaster] openItem clicked:" << path << "isDir:" << isDir;
    if (isDir) {
        loadDirectory(path);
    } else {
        openFile(path);
    }
}

void FileSystemModel::deleteItem(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists())
        return;

    if (fi.isDir()) {
        QDir dir(path);
        dir.removeRecursively();
    } else {
        QFile::remove(path);
    }
    loadDirectory(m_currentPath);
}

void FileSystemModel::renameItem(const QString &oldPath, const QString &newName)
{
    QFileInfo fi(oldPath);
    if (!fi.exists())
        return;

    QString newPath = fi.absolutePath() + QDir::separator() + newName;
    QFile::rename(oldPath, newPath);
    loadDirectory(m_currentPath);
}

void FileSystemModel::openInTerminal(const QString &path)
{
    QFileInfo fi(path);
    QString dirPath = fi.isDir() ? fi.absoluteFilePath() : fi.absolutePath();

    if (!QProcess::startDetached(QStringLiteral("konsole"), QStringList() << QStringLiteral("--workdir") << dirPath)) {
        if (!QProcess::startDetached(QStringLiteral("kitty"), QStringList() << QStringLiteral("--directory") << dirPath)) {
            QProcess::startDetached(QStringLiteral("x-terminal-emulator"), QStringList(), dirPath);
        }
    }
}

QString FileSystemModel::getFreeSpaceForPath(const QString &path)
{
    if (path.isEmpty())
        return QString();
    QFileInfo fi(path);
    if (!fi.exists())
        return QString();

    QStorageInfo storage(path);
    if (storage.isValid() && storage.isReady()) {
        return formatSize(storage.bytesAvailable()) + QStringLiteral(" libres");
    }
    return QString();
}

void FileSystemModel::searchFiles(const QString &query)
{
    m_pendingQuery = query.trimmed();
    if (m_pendingQuery.isEmpty()) {
        m_searchTimer->stop();
        if (m_searchProcess->state() != QProcess::NotRunning) {
            m_searchProcess->kill();
        }
        loadDirectory(m_currentPath);
        return;
    }

    // Debounce search input by 150ms to keep UI 60 FPS smooth
    m_searchTimer->start(150);
}

void FileSystemModel::performSearch()
{
    if (m_pendingQuery.isEmpty())
        return;

    beginResetModel();
    m_items.clear();
    m_addedSearchPaths.clear();

    // 1. Instant sub-millisecond search in active folder
    QDir dir(m_currentPath);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    const QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &fi : entries) {
        if (fi.fileName().contains(m_pendingQuery, Qt::CaseInsensitive)) {
            FileItem item;
            item.name = fi.fileName();
            item.path = fi.absoluteFilePath();
            item.isDir = fi.isDir();
            item.sizeStr = item.isDir ? QStringLiteral("Carpeta") : formatSize(fi.size());
            item.typeStr = fi.suffix().isEmpty() ? (item.isDir ? QStringLiteral("Carpeta") : QStringLiteral("Archivo")) : fi.suffix().toUpper();

            if (item.isDir) {
                item.iconName = QStringLiteral("📁");
            } else {
                QString ext = fi.suffix().toLower();
                if (ext == QLatin1String("mp4") || ext == QLatin1String("mkv") || ext == QLatin1String("avi") || ext == QLatin1String("mov")
                    || ext == QLatin1String("webm") || ext == QLatin1String("flv") || ext == QLatin1String("wmv") || ext == QLatin1String("m4v")) {
                    item.iconName = QStringLiteral("🎥");
                } else if (ext == QLatin1String("png") || ext == QLatin1String("jpg") || ext == QLatin1String("jpeg") || ext == QLatin1String("gif")
                           || ext == QLatin1String("svg") || ext == QLatin1String("webp") || ext == QLatin1String("bmp")) {
                    item.iconName = QStringLiteral("🖼️");
                } else if (ext == QLatin1String("mp3") || ext == QLatin1String("wav") || ext == QLatin1String("flac") || ext == QLatin1String("aac")
                           || ext == QLatin1String("ogg") || ext == QLatin1String("m4a")) {
                    item.iconName = QStringLiteral("🎵");
                } else if (ext == QLatin1String("pdf")) {
                    item.iconName = QStringLiteral("📕");
                } else if (ext == QLatin1String("zip") || ext == QLatin1String("tar") || ext == QLatin1String("gz") || ext == QLatin1String("7z")
                           || ext == QLatin1String("rar")) {
                    item.iconName = QStringLiteral("📦");
                } else if (ext == QLatin1String("cpp") || ext == QLatin1String("c") || ext == QLatin1String("h") || ext == QLatin1String("py")
                           || ext == QLatin1String("rs") || ext == QLatin1String("js") || ext == QLatin1String("ts") || ext == QLatin1String("sh")) {
                    item.iconName = QStringLiteral("📝");
                } else if (fi.isExecutable()) {
                    item.iconName = QStringLiteral("⚙️");
                } else {
                    item.iconName = QStringLiteral("📄");
                }
            }
            m_items.append(item);
            m_addedSearchPaths.insert(item.path);
        }
    }

    endResetModel();
    Q_EMIT itemCountChanged();

    // 2. Asynchronously run gfaster-rust in background
    if (m_searchProcess->state() != QProcess::NotRunning) {
        m_searchProcess->kill();
        m_searchProcess->waitForFinished(50);
    }

    QString rustBinary = QStringLiteral("gfaster-rust");
    if (!QFile::exists(QStringLiteral("/usr/bin/gfaster-rust"))) {
        if (QFile::exists(QDir::homePath() + QStringLiteral("/.local/bin/gfaster-rust"))) {
            rustBinary = QDir::homePath() + QStringLiteral("/.local/bin/gfaster-rust");
        } else if (QFile::exists(QStringLiteral("/run/media/starlord/Datos/fork-baloo/baloo-rust/target/release/gfaster-rust"))) {
            rustBinary = QStringLiteral("/run/media/starlord/Datos/fork-baloo/baloo-rust/target/release/gfaster-rust");
        }
    }

    m_searchProcess->start(rustBinary, QStringList() << QStringLiteral("search") << m_pendingQuery << QStringLiteral("--raw"));
}

void FileSystemModel::onSearchProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    QString output = QString::fromUtf8(m_searchProcess->readAllStandardOutput());
    const QStringList lines = output.split(QLatin1Char('\n'));

    bool addedNew = false;
    for (const QString &line : lines) {
        QString filePath = line.trimmed();
        if (!filePath.isEmpty() && !m_addedSearchPaths.contains(filePath)) {
            QFileInfo fi(filePath);
            if (fi.exists()) {
                FileItem item;
                item.name = fi.fileName();
                item.path = fi.absoluteFilePath();
                item.isDir = fi.isDir();
                item.sizeStr = item.isDir ? QStringLiteral("Carpeta") : formatSize(fi.size());
                item.typeStr = fi.suffix().isEmpty() ? (item.isDir ? QStringLiteral("Carpeta") : QStringLiteral("Archivo")) : fi.suffix().toUpper();

                if (item.isDir) {
                    item.iconName = QStringLiteral("📁");
                } else {
                    QString ext = fi.suffix().toLower();
                    if (ext == QLatin1String("mp4") || ext == QLatin1String("mkv") || ext == QLatin1String("avi") || ext == QLatin1String("mov")
                        || ext == QLatin1String("webm") || ext == QLatin1String("flv") || ext == QLatin1String("wmv") || ext == QLatin1String("m4v")) {
                        item.iconName = QStringLiteral("🎥");
                    } else if (ext == QLatin1String("png") || ext == QLatin1String("jpg") || ext == QLatin1String("jpeg") || ext == QLatin1String("gif")
                               || ext == QLatin1String("svg") || ext == QLatin1String("webp") || ext == QLatin1String("bmp")) {
                        item.iconName = QStringLiteral("🖼️");
                    } else if (ext == QLatin1String("mp3") || ext == QLatin1String("wav") || ext == QLatin1String("flac") || ext == QLatin1String("aac")
                               || ext == QLatin1String("ogg") || ext == QLatin1String("m4a")) {
                        item.iconName = QStringLiteral("🎵");
                    } else if (ext == QLatin1String("pdf")) {
                        item.iconName = QStringLiteral("📕");
                    } else if (ext == QLatin1String("zip") || ext == QLatin1String("tar") || ext == QLatin1String("gz") || ext == QLatin1String("7z")
                               || ext == QLatin1String("rar")) {
                        item.iconName = QStringLiteral("📦");
                    } else if (ext == QLatin1String("cpp") || ext == QLatin1String("c") || ext == QLatin1String("h") || ext == QLatin1String("py")
                               || ext == QLatin1String("rs") || ext == QLatin1String("js") || ext == QLatin1String("ts") || ext == QLatin1String("sh")) {
                        item.iconName = QStringLiteral("📝");
                    } else if (fi.isExecutable()) {
                        item.iconName = QStringLiteral("⚙️");
                    } else {
                        item.iconName = QStringLiteral("📄");
                    }
                }
                if (!addedNew) {
                    beginResetModel();
                    addedNew = true;
                }
                m_items.append(item);
                m_addedSearchPaths.insert(filePath);
            }
        }
    }

    if (addedNew) {
        endResetModel();
        Q_EMIT itemCountChanged();
    }
}

void FileSystemModel::compactDatabase()
{
    QProcess proc;
    proc.start(QStringLiteral("balooctl"), QStringList() << QStringLiteral("compact"));
    if (proc.waitForFinished(5000)) {
        Q_EMIT compactFinished(true, QStringLiteral("Base de datos LMDB compactada con éxito."));
    } else {
        Q_EMIT compactFinished(false, QStringLiteral("Error al ejecutar la compactación."));
    }
}

void FileSystemModel::loadDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) return;

    beginResetModel();
    m_currentPath = dir.absolutePath();
    m_items.clear();

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);

    const QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &fi : entries) {
        FileItem item;
        item.name = fi.fileName();
        item.path = fi.absoluteFilePath();
        item.isDir = fi.isDir();
        item.sizeStr = item.isDir ? QStringLiteral("Carpeta") : formatSize(fi.size());
        item.typeStr = fi.suffix().isEmpty() ? (item.isDir ? QStringLiteral("Carpeta") : QStringLiteral("Archivo")) : fi.suffix().toUpper();

        if (item.isDir) {
            item.iconName = QStringLiteral("📁");
        } else {
            QString ext = fi.suffix().toLower();
            if (ext == QLatin1String("mp4") || ext == QLatin1String("mkv") || ext == QLatin1String("avi") || ext == QLatin1String("mov")
                || ext == QLatin1String("webm") || ext == QLatin1String("flv") || ext == QLatin1String("wmv") || ext == QLatin1String("m4v")) {
                item.iconName = QStringLiteral("🎥");
            } else if (ext == QLatin1String("png") || ext == QLatin1String("jpg") || ext == QLatin1String("jpeg") || ext == QLatin1String("gif")
                       || ext == QLatin1String("svg") || ext == QLatin1String("webp") || ext == QLatin1String("bmp")) {
                item.iconName = QStringLiteral("🖼️");
            } else if (ext == QLatin1String("mp3") || ext == QLatin1String("wav") || ext == QLatin1String("flac") || ext == QLatin1String("aac")
                       || ext == QLatin1String("ogg") || ext == QLatin1String("m4a")) {
                item.iconName = QStringLiteral("🎵");
            } else if (ext == QLatin1String("pdf")) {
                item.iconName = QStringLiteral("📕");
            } else if (ext == QLatin1String("zip") || ext == QLatin1String("tar") || ext == QLatin1String("gz") || ext == QLatin1String("7z")
                       || ext == QLatin1String("rar")) {
                item.iconName = QStringLiteral("📦");
            } else if (ext == QLatin1String("cpp") || ext == QLatin1String("c") || ext == QLatin1String("h") || ext == QLatin1String("py")
                       || ext == QLatin1String("rs") || ext == QLatin1String("js") || ext == QLatin1String("ts") || ext == QLatin1String("sh")
                       || ext == QLatin1String("json") || ext == QLatin1String("md") || ext == QLatin1String("txt")) {
                item.iconName = QStringLiteral("📝");
            } else if (fi.isExecutable()) {
                item.iconName = QStringLiteral("⚙️");
            } else {
                item.iconName = QStringLiteral("📄");
            }
        }
        m_items.append(item);
    }

    endResetModel();
    Q_EMIT currentPathChanged();
    Q_EMIT itemCountChanged();

    // Update free space
    QStorageInfo storage(m_currentPath);
    m_freeSpaceStr = formatSize(storage.bytesAvailable()) + QStringLiteral(" libres");
    Q_EMIT freeSpaceStrChanged();
}

QString FileSystemModel::formatSize(qint64 bytes) const
{
    if (bytes < 1024) return QString::number(bytes) + QStringLiteral(" B");
    double kb = bytes / 1024.0;
    if (kb < 1024) return QString::number(kb, 'f', 1) + QStringLiteral(" KB");
    double mb = kb / 1024.0;
    if (mb < 1024) return QString::number(mb, 'f', 1) + QStringLiteral(" MB");
    double gb = mb / 1024.0;
    return QString::number(gb, 'f', 1) + QStringLiteral(" GB");
}
