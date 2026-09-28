/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#if __has_include(<monitor.h>)
#include <monitor.h>
#endif

#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_org_kde_baloo_experimental()
{
    qmlRegisterModule("org.kde.baloo.experimental", 0, 0);
    QT_WARNING_PUSH QT_WARNING_DISABLE_DEPRECATED qmlRegisterTypesAndRevisions<Baloo::Monitor>("org.kde.baloo.experimental", 0);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {
                return &Baloo::staticMetaObject;
            },
            "Baloo");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&Baloo::staticMetaObject, "org.kde.baloo.experimental", 0, nullptr, &BalooForeign::staticMetaObject, nullptr);
    qmlRegisterEnum<Baloo::IndexerState>("Baloo::IndexerState");
    QT_WARNING_POP
    qmlRegisterModule("org.kde.baloo.experimental", 0, 1);
}

static const QQmlModuleRegistration orgkdebalooexperimentalRegistration("org.kde.baloo.experimental", qml_register_types_org_kde_baloo_experimental);
