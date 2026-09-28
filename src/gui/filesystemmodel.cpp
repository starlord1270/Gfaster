#include "filesystemmodel.h"
#include <QStorageInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QProcess>

FileSystemModel::FileSystemModel(QObject *parent)
    : QAbstractListModel(parent)
{
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

void FileSystemModel::searchFiles(const QString &query)
{
    if (query.trimmed().isEmpty()) {
        loadDirectory(m_currentPath);
        return;
    }

    beginResetModel();
    m_items.clear();

    // Call gfaster-rust binary for instant parallel search
    QProcess proc;
    proc.start(QStringLiteral("gfaster-rust"), QStringList() << QStringLiteral("search") << query);
    if (proc.waitForFinished(1500)) {
        QString output = QString::fromUtf8(proc.readAllStandardOutput());
        const QStringList lines = output.split(QLatin1Char('\n'));
        for (const QString &line : lines) {
            if (line.contains(QLatin1String(" /"))) {
                int startIdx = line.indexOf(QLatin1Char('/'));
                int endIdx = line.lastIndexOf(QLatin1Char('('));
                if (startIdx != -1 && endIdx > startIdx) {
                    QString filePath = line.mid(startIdx, endIdx - startIdx).trimmed();
                    QFileInfo fi(filePath);
                    if (fi.exists()) {
                        FileItem item;
                        item.name = fi.fileName();
                        item.path = fi.absoluteFilePath();
                        item.isDir = fi.isDir();
                        item.sizeStr = item.isDir ? QStringLiteral("Carpeta") : formatSize(fi.size());
                        item.typeStr = fi.suffix().isEmpty() ? (item.isDir ? QStringLiteral("Directorio") : QStringLiteral("Archivo")) : fi.suffix().toUpper();
                        item.iconName = item.isDir ? QStringLiteral("folder") : QStringLiteral("document");
                        m_items.append(item);
                    }
                }
            }
        }
    }

    endResetModel();
    Q_EMIT itemCountChanged();
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
        item.iconName = item.isDir ? QStringLiteral("folder") : QStringLiteral("document");
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
