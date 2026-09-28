import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GFaster 1.0

ApplicationWindow {
    id: window
    width: 1360
    height: 850
    visible: true
    title: "GFaster File Manager"
    color: bgBase

    FileSystemModel {
        id: fsModel
    }

    // Palette tokens (Wine Red & Obsidian Black)
    readonly property color bgBase: "#0b0809"
    readonly property color bgSurface: "#140e11"
    readonly property color bgCard: "#1d1418"
    readonly property color bgCardHover: "#2d1d24"
    readonly property color wineRed: "#7a001e"
    readonly property color wineRedBright: "#a8002a"
    readonly property color wineRedNeon: "#ff2a5f"
    readonly property color borderGlow: "#3d101a"
    readonly property color textMain: "#f5f5f7"
    readonly property color textMuted: "#b89da7"

    // Dynamic Zoom Level for Icons & Text
    property real zoomScale: 1.0

    Item {
        anchors.fill: parent

        RowLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 24  // Spacing separating sidebar from main content pane!

            // Sidebar Panel
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 260
                color: bgSurface
                radius: 16
                border.color: borderGlow
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    // App Brand Header
                    RowLayout {
                        spacing: 12
                        Image {
                            source: "file:///run/media/starlord/Datos/fork-baloo/gfaster_logo.png"
                            Layout.preferredWidth: 36
                            Layout.preferredHeight: 36
                            fillMode: Image.PreserveAspectFit
                        }
                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: "GFaster"
                                color: textMain
                                font.pixelSize: 18
                                font.bold: true
                            }
                            Text {
                                text: "File Explorer"
                                color: wineRedNeon
                                font.pixelSize: 11
                                font.weight: Font.DemiBold
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: borderGlow
                    }

                    // Scrollable Category List
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        ScrollBar.vertical.policy: ScrollBar.AsNeeded

                        ColumnLayout {
                            width: parent.width - 10
                            spacing: 16

                            // SECTION 1: Lugares
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "Lugares"
                                    color: textMain
                                    font.pixelSize: 13
                                    font.bold: true
                                    Layout.leftMargin: 8
                                    Layout.bottomMargin: 4
                                }

                                SidebarItem { itemText: "💻 Equipo"; path: "/" }
                                SidebarItem { itemText: "🏠 starlord"; path: "/home/starlord" }
                                SidebarItem { itemText: "🖥️ Desktop"; path: "/home/starlord/Desktop" }
                                SidebarItem { itemText: "🕒 Recientes"; path: "/home/starlord/Downloads" }
                                SidebarItem { itemText: "🗑️ Papelera"; path: "/home/starlord/.local/share/Trash/files" }
                            }

                            // SECTION 2: Dispositivos
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "Dispositivos"
                                    color: textMain
                                    font.pixelSize: 13
                                    font.bold: true
                                    Layout.leftMargin: 8
                                    Layout.bottomMargin: 4
                                }

                                SidebarItem { itemText: "💽 Sistema de archivos"; path: "/" }
                                SidebarItem { itemText: "💽 Datos"; path: "/run/media/starlord/Datos" }
                                SidebarItem { itemText: "📱 Infinix X682B"; path: "/home/starlord/infinix_mnt" }
                                SidebarItem { itemText: "💽 universidad"; path: "/home/starlord/Universidad" }
                            }

                            // SECTION 3: Red
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "Red"
                                    color: textMain
                                    font.pixelSize: 13
                                    font.bold: true
                                    Layout.leftMargin: 8
                                    Layout.bottomMargin: 4
                                }

                                SidebarItem { itemText: "🌐 Navegar por la red"; path: "/" }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: borderGlow
                    }

                    // Compact DB Button
                    Button {
                        Layout.fillWidth: true
                        height: 38
                        text: "🧹 Compactar DB"
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.pixelSize: 12
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: parent.hovered ? wineRedNeon : wineRed
                            radius: 10
                        }
                        onClicked: fsModel.compactDatabase()
                    }

                    // Disk Usage Widget
                    Rectangle {
                        Layout.fillWidth: true
                        height: 48
                        color: bgCard
                        radius: 10

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 4
                            Text {
                                text: "Disco: " + fsModel.freeSpaceStr
                                color: textMuted
                                font.pixelSize: 11
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                height: 5
                                radius: 2.5
                                color: "#2d1219"
                                Rectangle {
                                    width: parent.width * 0.6
                                    height: parent.height
                                    radius: 2.5
                                    color: wineRedNeon
                                }
                            }
                        }
                    }
                }
            }

            // Main Content Area
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 16

                // Top Toolbar / Path Breadcrumb
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 14

                    Button {
                        text: "⬅ Volver"
                        contentItem: Text { text: parent.text; color: textMain; font.bold: true }
                        background: Rectangle {
                            color: parent.hovered ? bgCardHover : bgCard
                            radius: 10
                            border.width: 0
                        }
                        onClicked: fsModel.openParentDir()
                    }

                    // Current Path Bar
                    Rectangle {
                        Layout.fillWidth: true
                        height: 40
                        color: bgSurface
                        radius: 10

                        Text {
                            anchors.centerIn: parent
                            text: fsModel.currentPath
                            color: textMain
                            font.pixelSize: 13
                            font.bold: true
                        }
                    }

                    // Zoom Controls (Aumentar / Reducir Letra e Iconos)
                    RowLayout {
                        spacing: 4

                        Button {
                            text: "➖ Zoom"
                            contentItem: Text {
                                text: parent.text
                                color: textMain
                                font.pixelSize: 11
                                font.bold: true
                            }
                            background: Rectangle {
                                color: parent.hovered ? bgCardHover : bgCard
                                radius: 8
                            }
                            onClicked: zoomScale = Math.max(0.6, zoomScale - 0.15)
                        }

                        Text {
                            text: Math.round(zoomScale * 100) + "%"
                            color: wineRedNeon
                            font.pixelSize: 11
                            font.bold: true
                            Layout.leftMargin: 2
                            Layout.rightMargin: 2
                        }

                        Button {
                            text: "➕ Zoom"
                            contentItem: Text {
                                text: parent.text
                                color: textMain
                                font.pixelSize: 11
                                font.bold: true
                            }
                            background: Rectangle {
                                color: parent.hovered ? bgCardHover : bgCard
                                radius: 8
                            }
                            onClicked: zoomScale = Math.min(2.0, zoomScale + 0.15)
                        }
                    }

                    // Search Input
                    TextField {
                        id: searchBox
                        placeholderText: "🔍 Buscar con GFaster Rust..."
                        placeholderTextColor: textMuted
                        color: textMain
                        font.pixelSize: 13
                        Layout.preferredWidth: 260
                        background: Rectangle {
                            color: bgSurface
                            radius: 10
                            border.color: searchBox.activeFocus ? wineRedNeon : "transparent"
                            border.width: 1
                        }
                        onTextChanged: fsModel.searchFiles(text)
                    }
                }

                // Grid View of Folder Cards with Right ScrollBar
                GridView {
                    id: gridView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth: Math.round(175 * zoomScale)
                    cellHeight: Math.round(160 * zoomScale)
                    clip: true
                    model: fsModel
                    highlight: null
                    highlightFollowsCurrentItem: false

                    // Right ScrollBar for vertical scrolling
                    ScrollBar.vertical: ScrollBar {
                        id: verticalScrollBar
                        active: true
                        policy: ScrollBar.AlwaysOn
                        contentItem: Rectangle {
                            implicitWidth: 8
                            implicitHeight: 100
                            radius: 4
                            color: verticalScrollBar.pressed ? wineRedNeon : (verticalScrollBar.hovered ? wineRedBright : wineRed)
                        }
                        background: Rectangle {
                            implicitWidth: 8
                            color: "#160e12"
                            radius: 4
                        }
                    }

                    delegate: Rectangle {
                        id: cardDelegate
                        width: Math.round(160 * zoomScale)
                        height: Math.round(145 * zoomScale)
                        color: mouseArea.containsMouse ? bgCardHover : "transparent"
                        radius: Math.round(14 * zoomScale)
                        border.color: mouseArea.containsMouse ? wineRedNeon : "transparent"
                        border.width: mouseArea.containsMouse ? 1 : 0

                        // Extension Badge Pill (Top-Right of file card)
                        Rectangle {
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.topMargin: Math.round(8 * zoomScale)
                            anchors.rightMargin: Math.round(8 * zoomScale)
                            visible: !isDir && typeStr !== "" && typeStr !== "Carpeta"
                            color: wineRed
                            radius: Math.round(4 * zoomScale)
                            implicitWidth: extLabel.implicitWidth + Math.round(8 * zoomScale)
                            implicitHeight: extLabel.implicitHeight + Math.round(4 * zoomScale)
                            z: 2

                            Text {
                                id: extLabel
                                anchors.centerIn: parent
                                text: typeStr
                                color: "#ffffff"
                                font.pixelSize: Math.max(7, Math.round(9 * zoomScale))
                                font.bold: true
                            }
                        }

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: Math.round(6 * zoomScale)
                            width: parent.width - Math.round(20 * zoomScale)

                            Text {
                                text: iconName
                                font.pixelSize: Math.round(44 * zoomScale)
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Text {
                                text: name
                                color: textMain
                                font.pixelSize: Math.max(9, Math.round(12 * zoomScale))
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Text {
                                text: sizeStr
                                color: wineRedNeon
                                font.pixelSize: Math.max(8, Math.round(10 * zoomScale))
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.LeftButton | Qt.RightButton

                            onClicked: (mouse) => {
                                if (mouse.button === Qt.LeftButton) {
                                    fsModel.openItem(path, isDir)
                                } else if (mouse.button === Qt.RightButton) {
                                    itemContextMenu.popup()
                                }
                            }

                            onDoubleClicked: (mouse) => {
                                if (mouse.button === Qt.LeftButton) {
                                    fsModel.openItem(path, isDir)
                                }
                            }

                            Menu {
                                id: itemContextMenu

                                MenuItem {
                                    text: isDir ? "📂 Abrir Carpeta" : "▶ Abrir Archivo"
                                    onTriggered: fsModel.openItem(path, isDir)
                                }

                                Menu {
                                    id: openWithMenu
                                    title: "🚀 Abrir con..."
                                    visible: !isDir

                                    MenuItem {
                                        text: "⚙️ Abrir con el sistema (KDE)..."
                                        onTriggered: fsModel.openWithSystemDialog(path)
                                    }

                                    MenuItem {
                                        text: "🔍 Buscador rápido de aplicaciones..."
                                        onTriggered: appSelectorDialog.openForFile(path, name)
                                    }

                                    MenuSeparator {}

                                    Instantiator {
                                        active: openWithMenu.opened
                                        model: openWithMenu.opened ? fsModel.getOpenWithApps(path) : []
                                        onObjectAdded: (index, object) => openWithMenu.insertItem(index + 3, object)
                                        onObjectRemoved: (index, object) => openWithMenu.removeItem(object)
                                        delegate: MenuItem {
                                            required property var modelData
                                            text: (modelData.isRecommended ? "⭐ " : "📱 ") + modelData.name
                                            onTriggered: fsModel.launchWithApp(path, modelData.cmd)
                                        }
                                    }
                                }

                                Menu {
                                    title: "🔍 Tamaño de letras e iconos"
                                    MenuItem {
                                        text: "➕ Aumentar (Zoom +)"
                                        onTriggered: zoomScale = Math.min(2.0, zoomScale + 0.15)
                                    }
                                    MenuItem {
                                        text: "➖ Reducir (Zoom -)"
                                        onTriggered: zoomScale = Math.max(0.6, zoomScale - 0.15)
                                    }
                                    MenuItem {
                                        text: "🔄 Normal (100%)"
                                        onTriggered: zoomScale = 1.0
                                    }
                                }

                                MenuItem {
                                    text: "🖥️ Abrir en Terminal"
                                    onTriggered: fsModel.openInTerminal(path)
                                }

                                MenuSeparator {}

                                MenuItem {
                                    text: "🗑️ Eliminar"
                                    onTriggered: fsModel.deleteItem(path)
                                }
                            }
                        }

                    }
                }
            }
        }
    }

    // Modal Dialog to Search & Choose Any System Application
    Dialog {
        id: appSelectorDialog
        title: "Elegir aplicación del sistema"
        modal: true
        focus: true
        width: 580
        height: 520
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)

        property string targetPath: ""
        property string targetName: ""
        property var allAppsList: []

        function openForFile(filePath, fileName) {
            targetPath = filePath
            targetName = fileName
            allAppsList = fsModel.getOpenWithApps(filePath)
            appSearchInput.text = ""
            open()
        }

        background: Rectangle {
            color: bgSurface
            radius: 16
            border.color: borderGlow
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: 12

            Text {
                text: "🚀 Abrir \"" + targetName + "\" con:"
                color: textMain
                font.pixelSize: 16
                font.bold: true
            }

            TextField {
                id: appSearchInput
                placeholderText: "🔍 Buscar aplicación (ej: Chrome, Edge, Firefox, Code, VLC)..."
                placeholderTextColor: textMuted
                color: textMain
                font.pixelSize: 13
                Layout.fillWidth: true
                background: Rectangle {
                    color: bgCard
                    radius: 10
                    border.color: appSearchInput.activeFocus ? wineRedNeon : borderGlow
                    border.width: 1
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ListView {
                    id: appListView
                    width: parent.width
                    spacing: 6
                    model: {
                        var query = appSearchInput.text.trimmed().toLowerCase()
                        if (query === "") return allAppsList
                        return allAppsList.filter(function(app) {
                            return app.name.toLowerCase().indexOf(query) !== -1 || app.cmd.toLowerCase().indexOf(query) !== -1
                        })
                    }

                    delegate: Rectangle {
                        width: appListView.width - 12
                        height: 46
                        color: itemMouseArea.containsMouse ? wineRed : bgCard
                        radius: 10

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 12

                            Text {
                                text: modelData.isRecommended ? "⭐" : "📱"
                                font.pixelSize: 16
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Text {
                                    text: modelData.name
                                    color: textMain
                                    font.pixelSize: 13
                                    font.bold: true
                                    elide: Text.ElideRight
                                }

                                Text {
                                    text: modelData.cmd
                                    color: textMuted
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                }
                            }

                            Button {
                                text: "Abrir"
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                    font.bold: true
                                    font.pixelSize: 11
                                }
                                background: Rectangle {
                                    color: parent.hovered ? wineRedBright : wineRedNeon
                                    radius: 6
                                }
                                onClicked: {
                                    fsModel.launchWithApp(targetPath, modelData.cmd)
                                    appSelectorDialog.close()
                                }
                            }
                        }

                        MouseArea {
                            id: itemMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onDoubleClicked: {
                                fsModel.launchWithApp(targetPath, modelData.cmd)
                                appSelectorDialog.close()
                            }
                        }
                    }
                }
            }
        }
    }

    // Helper component for sidebar links
    component SidebarItem : Button {
        property string path: ""
        property string itemText: ""
        property string spaceText: fsModel.getFreeSpaceForPath(path)

        Layout.fillWidth: true
        height: spaceText !== "" ? 44 : 34

        contentItem: ColumnLayout {
            spacing: 1
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10

            Text {
                text: itemText
                color: parent.parent.hovered ? textMain : textMuted
                font.pixelSize: 13
                font.weight: parent.parent.hovered ? Font.Medium : Font.Normal
            }

            Text {
                text: spaceText
                color: wineRedNeon
                font.pixelSize: 10
                font.weight: Font.DemiBold
                visible: spaceText !== ""
            }
        }

        background: Rectangle {
            color: parent.hovered ? (fsModel.currentPath === path ? wineRed : bgCardHover) : (fsModel.currentPath === path ? bgCard : "transparent")
            radius: 8
        }

        onClicked: {
            if (path !== "") fsModel.openDir(path)
        }
    }
}



