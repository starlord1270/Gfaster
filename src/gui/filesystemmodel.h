#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QDir>
#include <QProcess>

struct FileItem {
    QString name;
    QString path;
    QString sizeStr;
    QString typeStr;
    bool isDir;
    QString iconName;
};

class FileSystemModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
    Q_PROPERTY(QString freeSpaceStr READ freeSpaceStr NOTIFY freeSpaceStrChanged)

public:
    enum FileRoles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        SizeRole,
        TypeRole,
        IsDirRole,
        IconNameRole
    };

    explicit FileSystemModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString currentPath() const { return m_currentPath; }
    void setCurrentPath(const QString &path);

    int itemCount() const { return m_items.size(); }
    QString freeSpaceStr() const { return m_freeSpaceStr; }

    Q_INVOKABLE void openDir(const QString &path);
    Q_INVOKABLE void openParentDir();
    Q_INVOKABLE void searchFiles(const QString &query);
    Q_INVOKABLE void compactDatabase();

signals:
    void currentPathChanged();
    void itemCountChanged();
    void freeSpaceStrChanged();
    void compactFinished(bool success, const QString &message);

private:
    void loadDirectory(const QString &path);
    QString formatSize(qint64 bytes) const;

    QString m_currentPath;
    QString m_freeSpaceStr;
    QList<FileItem> m_items;
};

#endif // FILESYSTEMMODEL_H
