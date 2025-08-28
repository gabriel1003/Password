import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Accessibility 1.0

ApplicationWindow {
    id: app
    width: 480
    height: 720
    visible: true
    title: qsTr("Gerador & Gestor de Senhas")


    Accessible.role: Accessible.Window
    Accessible.name: qsTr("Aplicativo Gerador e Gestor de Senhas")

    header: ToolBar {
        contentHeight: implicitHeight
        Label {
            text: app.title
            font.pixelSize: 16
            padding: 12
            Accessible.role: Accessible.StaticText
            Accessible.name: app.title
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: Qt.resolvedUrl("welcome.qml")
        Accessible.role: Accessible.Pane
        Accessible.name: qsTr("Área de conteúdo")
    }

    // Conexões globais de navegação (opcional: chamadas pelo backend)
    Connections {
        target: stack.currentItem
        // Exemplos: você pode interceptar sinais aqui se quiser
    }
