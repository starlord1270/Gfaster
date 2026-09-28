#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QAbstractListModel>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QSet>
#include <QTimer>
#include <QUrl>

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
    Q_INVOKABLE void openFile(const QString &path);
    Q_INVOKABLE void openItem(const QString &path, bool isDir);
    Q_INVOKABLE void deleteItem(const QString &path);
    Q_INVOKABLE void renameItem(const QString &oldPath, const QString &newName);
    Q_INVOKABLE void openInTerminal(const QString &path);
    Q_INVOKABLE QString getFreeSpaceForPath(const QString &path);
    Q_INVOKABLE QVariantList getOpenWithApps(const QString &path);
    Q_INVOKABLE void launchWithApp(const QString &path, const QString &execCmd);
    Q_INVOKABLE void searchFiles(const QString &query);
    Q_INVOKABLE void compactDatabase();

Q_SIGNALS:
    void currentPathChanged();
    void itemCountChanged();
    void freeSpaceStrChanged();
    void compactFinished(bool success, const QString &message);

private Q_SLOTS:
    void performSearch();
    void onSearchProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void loadDirectory(const QString &path);
    QString formatSize(qint64 bytes) const;

    QString m_currentPath;
    QString m_freeSpaceStr;
    QList<FileItem> m_items;

    QProcess *m_searchProcess;
    QTimer *m_searchTimer;
    QString m_pendingQuery;
    QSet<QString> m_addedSearchPaths;
};

#endif // FILESYSTEMMODEL_H
