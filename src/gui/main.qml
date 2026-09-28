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

    Item {
        anchors.fill: parent

        RowLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 24  // Spacing separating sidebar from main content pane!

            // Sidebar Panel
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 250
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

                                SidebarItem { text: "💻 Equipo"; path: "/" }
                                SidebarItem { text: "🏠 starlord"; path: "/home/starlord" }
                                SidebarItem { text: "🖥️ Desktop"; path: "/home/starlord/Desktop" }
                                SidebarItem { text: "🕒 Recientes"; path: "/home/starlord/Downloads" }
                                SidebarItem { text: "🗑️ Papelera"; path: "/home/starlord/.local/share/Trash/files" }
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

                                SidebarItem { text: "💽 Sistema de archivos"; path: "/" }
                                SidebarItem { text: "💽 Datos"; path: "/run/media/starlord/Datos" }
                                SidebarItem { text: "📱 Infinix X682B"; path: "/home/starlord/infinix_mnt" }
                                SidebarItem { text: "💽 universidad"; path: "/home/starlord/Universidad" }
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

                                SidebarItem { text: "🌐 Navegar por la red"; path: "/" }
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

                    // Search Input
                    TextField {
                        id: searchBox
                        placeholderText: "🔍 Buscar con GFaster Rust..."
                        placeholderTextColor: textMuted
                        color: textMain
                        font.pixelSize: 13
                        Layout.preferredWidth: 300
                        background: Rectangle {
                            color: bgSurface
                            radius: 10
                            border.color: searchBox.activeFocus ? wineRedNeon : "transparent"
                            border.width: 1
                        }
                        onTextChanged: fsModel.searchFiles(text)
                    }
                }

                // Grid View of Folder Cards (NO OUTLINE BORDER)
                GridView {
                    id: gridView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth: 175
                    cellHeight: 155
                    clip: true
                    model: fsModel

                    delegate: Rectangle {
                        width: 160
                        height: 140
                        color: mouseArea.containsMouse ? bgCardHover : bgCard
                        radius: 14
                        border.width: 0  // NO OUTLINE BORDER!

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 8
                            width: parent.width - 20

                            Text {
                                text: isDir ? "📁" : "📄"
                                font.pixelSize: 44
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Text {
                                text: name
                                color: textMain
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Text {
                                text: sizeStr
                                color: wineRedNeon
                                font.pixelSize: 10
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onDoubleClicked: {
                                if (isDir) {
                                    fsModel.openDir(path)
                                }
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
        Layout.fillWidth: true
        height: 36
        contentItem: Text {
            text: parent.text
            color: parent.hovered ? textMain : textMuted
            font.pixelSize: 13
            font.weight: parent.hovered ? Font.Medium : Font.Normal
            verticalAlignment: Text.AlignVCenter
            leftPadding: 12
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

