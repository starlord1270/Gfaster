#include "filesystemmodel.h"
#include <QStorageInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QProcess>

FileSystemModel::FileSystemModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_searchProcess(new QProcess(this))
    , m_searchTimer(new QTimer(this))
{
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, &QTimer::timeout, this, &FileSystemModel::performSearch);
    connect(m_searchProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &FileSystemModel::onSearchProcessFinished);

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
    if (!fi.exists())
        return;

    if (fi.isExecutable() && !fi.isDir()) {
        QProcess::startDetached(path, QStringList());
    } else {
        bool ok = QProcess::startDetached(QStringLiteral("xdg-open"), QStringList() << path);
        if (!ok) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    }
}

void FileSystemModel::openItem(const QString &path, bool isDir)
{
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
