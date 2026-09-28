#include "filesystemmodel.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(QStringLiteral("GFaster"));
    app.setApplicationName(QStringLiteral("GFaster File Manager"));
    app.setWindowIcon(QIcon(QStringLiteral("/usr/share/gfaster/gfaster_logo.png")));

    qmlRegisterType<FileSystemModel>("GFaster", 1, 0, "FileSystemModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qt/qml/gfaster/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    QString qmlPath = QStringLiteral("/usr/share/gfaster/main.qml");
    if (!QFile::exists(qmlPath)) {
        qmlPath = QStringLiteral("/run/media/starlord/Datos/fork-baloo/src/gui/main.qml");
    }
    if (!QFile::exists(qmlPath)) {
        qmlPath = QDir::homePath() + QStringLiteral("/.local/share/gfaster/main.qml");
    }

    engine.load(QUrl::fromLocalFile(qmlPath));

    return app.exec();
}
