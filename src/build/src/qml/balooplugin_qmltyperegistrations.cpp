/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#if __has_include(<queryresultsmodel.h>)
#include <queryresultsmodel.h>
#endif

#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_org_kde_baloo()
{
    qmlRegisterModule("org.kde.baloo", 0, 0);
    QT_WARNING_PUSH QT_WARNING_DISABLE_DEPRECATED QMetaType::fromType<QAbstractItemModel *>().id();
    qmlRegisterEnum<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qmlRegisterEnum<QAbstractItemModel::CheckIndexOption>("QAbstractItemModel::CheckIndexOption");
    QMetaType::fromType<QAbstractListModel *>().id();
    qmlRegisterTypesAndRevisions<Query>("org.kde.baloo", 0);
    qmlRegisterTypesAndRevisions<QueryResultsModel>("org.kde.baloo", 0);
    QT_WARNING_POP
    qmlRegisterModule("org.kde.baloo", 0, 1);
}

static const QQmlModuleRegistration orgkdebalooRegistration("org.kde.baloo", qml_register_types_org_kde_baloo);
