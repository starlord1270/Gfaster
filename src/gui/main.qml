import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GFaster 1.0

ApplicationWindow {
    id: window
    width: 1280
    height: 800
    visible: true
    title: "GFaster File Manager"
    color: "#0f0b0c"

    FileSystemModel {
        id: fsModel
    }

    // Palette tokens (Wine Red & Black)
    readonly property color bgBase: "#0f0b0c"
    readonly property color bgSurface: "#171013"
    readonly property color bgCard: "#24181d"
    readonly property color bgCardHover: "#332128"
    readonly property color wineRed: "#800020"
    readonly property color wineRedBright: "#b3003b"
    readonly property color wineRedNeon: "#ff3366"
    readonly property color borderGlow: "#4a1220"
    readonly property color textMain: "#ffffff"
    readonly property color textMuted: "#d9b8c2"

    Rectangle {
        anchors.fill: parent
        color: bgBase

        // Gradient Glow
        RadialGradient {
            anchors.fill: parent
            visible: false
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // Sidebar
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 240
                color: bgSurface
                border.color: borderGlow
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 20

                    // Logo & App Name
                    RowLayout {
                        spacing: 12
                        Image {
                            source: "file:///run/media/starlord/Datos/fork-baloo/gfaster_logo.png"
                            Layout.preferredWidth: 36
                            Layout.preferredHeight: 36
                            fillMode: Image.PreserveAspectFit
                        }
                        ColumnLayout {
                            spacing: 0
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

                    // Navigation Links
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: "ACCESO RÁPIDO"
                            color: textMuted
                            font.pixelSize: 10
                            font.bold: true
                        }

                        Button {
                            Layout.fillWidth: true
                            text: "🏠  Inicio (~)"
                            contentItem: Text { text: parent.text; color: textMain; font.pixelSize: 13; font.bold: true }
                            background: Rectangle { color: parent.hovered ? bgCardHover : bgCard; radius: 10 }
                            onClicked: fsModel.openDir("/home")
                        }

                        Button {
                            Layout.fillWidth: true
                            text: "💻  Proyectos"
                            contentItem: Text { text: parent.text; color: textMain; font.pixelSize: 13; font.bold: true }
                            background: Rectangle { color: parent.hovered ? bgCardHover : bgCard; radius: 10 }
                            onClicked: fsModel.openDir("/run/media/starlord/Datos")
                        }

                        Button {
                            Layout.fillWidth: true
                            text: "📄  Documentos"
                            contentItem: Text { text: parent.text; color: textMain; font.pixelSize: 13; font.bold: true }
                            background: Rectangle { color: parent.hovered ? bgCardHover : bgCard; radius: 10 }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    // Maintenance Action
                    Button {
                        Layout.fillWidth: true
                        text: "🧹 Compactar DB"
                        contentItem: Text { text: parent.text; color: "#ffffff"; font.pixelSize: 13; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                        background: Rectangle { color: parent.hovered ? wineRedNeon : wineRed; radius: 10 }
                        onClicked: fsModel.compactDatabase()
                    }

                    // Storage Meter
                    Rectangle {
                        Layout.fillWidth: true
                        height: 54
                        color: bgCard
                        radius: 12
                        border.color: borderGlow

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 4
                            Text { text: "Disco: " + fsModel.freeSpaceStr; color: textMuted; font.pixelSize: 11 }
                            Rectangle {
                                Layout.fillWidth: true
                                height: 6
                                radius: 3
                                color: "#30151c"
                                Rectangle {
                                    width: parent.width * 0.65
                                    height: parent.height
                                    radius: 3
                                    color: wineRedNeon
                                }
                            }
                        }
                    }
                }
            }

            // Main Content Pane
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 16
                anchors.margins: 20

                // Header Search & Navigation Bar
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        text: "⬅ Volver"
                        contentItem: Text { text: parent.text; color: textMain; font.bold: true }
                        background: Rectangle { color: bgCard; radius: 8; border.color: borderGlow }
                        onClicked: fsModel.openParentDir()
                    }

                    // Path Breadcrumb
                    Rectangle {
                        Layout.fillWidth: true
                        height: 38
                        color: bgSurface
                        radius: 10
                        border.color: borderGlow

                        Text {
                            anchors.centerIn: parent
                            text: fsModel.currentPath
                            color: textMain
                            font.pixelSize: 13
                            font.bold: true
                        }
                    }

                    // Search Box
                    TextField {
                        id: searchBox
                        placeholderText: "🔍 Buscar con GFaster Rust..."
                        placeholderTextColor: textMuted
                        color: textMain
                        font.pixelSize: 13
                        Layout.preferredWidth: 320
                        background: Rectangle {
                            color: bgSurface
                            radius: 20
                            border.color: searchBox.activeFocus ? wineRedNeon : borderGlow
                            border.width: 1
                        }
                        onTextChanged: fsModel.searchFiles(text)
                    }
                }

                // Grid View of File Cards
                GridView {
                    id: gridView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth: 180
                    cellHeight: 160
                    clip: true
                    model: fsModel

                    delegate: Rectangle {
                        width: 164
                        height: 144
                        color: mouseArea.containsMouse ? bgCardHover : bgCard
                        radius: 16
                        border.color: mouseArea.containsMouse ? wineRedNeon : borderGlow
                        border.width: 1

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 8
                            width: parent.width - 24

                            Text {
                                text: isDir ? "📁" : "📄"
                                font.pixelSize: 42
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
}
