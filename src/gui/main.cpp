#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "filesystemmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName(QStringLiteral("GFaster"));
    app.setApplicationName(QStringLiteral("GFaster File Manager"));
    app.setWindowIcon(QIcon(QStringLiteral("/usr/share/gfaster/gfaster_logo.png")));

    qmlRegisterType<FileSystemModel>("GFaster", 1, 0, "FileSystemModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qt/qml/gfaster/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.load(QUrl::fromLocalFile(QStringLiteral("/usr/share/gfaster/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        // Fallback for local build folder
        engine.load(QUrl::fromLocalFile(QStringLiteral("./src/gui/main.qml")));
    }

    return app.exec();
}
