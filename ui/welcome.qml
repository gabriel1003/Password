import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Accessibility 1.0

Item {
    id: root
    anchors.fill: parent

    signal createLoginRequested()
    signal loginRequested()

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16
        width: Math.min(parent.width * 0.9, 420)

        // Mensagem de boas-vindas
        Label {
            id: welcomeText
            text: qsTr("Seja bem-vindo ao nosso gerador e gestor de senhas.\nFaça login ou crie um login.")
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Accessible.role: Accessible.StaticText
            Accessible.name: text
            Accessible.description: qsTr("Mensagem inicial com instruções para acessar ou criar login.")
        }

        RowLayout {
            spacing: 12
            Layout.alignment: Qt.AlignHCenter

            Button {
                id: createBtn
                text: qsTr("Criar login")
                focus: true
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Ir para a tela de criação de login.")
                onClicked: {
                    if (StackView.view)
                        StackView.view.push("created_login.qml")
                }
                shortcut: StandardKey.New
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Criar um novo login (atalho: Ctrl+N)")
            }

            Button {
                id: loginBtn
                text: qsTr("Fazer login")
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Ir para a tela de login.")
                onClicked: {
                    if (StackView.view)
                        StackView.view.push("login.qml")
                }
                shortcut: StandardKey.Open
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Fazer login (atalho: Ctrl+O)")
            }
        }
    }
}
