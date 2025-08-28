import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Accessibility 1.0

Item {
    id: root
    anchors.fill: parent

    property string username: "default"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12
        width: Math.min(parent.width * 0.9, 420)

        Label {
            id: title
            text: qsTr("Fazer login")
            font.pixelSize: 18
            Accessible.role: Accessible.Heading
            Accessible.name: text
        }

        Label {
            id: passLabel
            text: qsTr("Senha mestra")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        TextField {
            id: passField
            placeholderText: qsTr("Digite sua senha mestra")
            echoMode: TextInput.Password
            Layout.fillWidth: true
            focus: true
            activeFocusOnTab: true
            Accessible.role: Accessible.EditableText
            Accessible.name: passLabel.text
            Accessible.description: qsTr("Campo para digitar a senha do login.")
            labeledBy: passLabel
            onAccepted: loginBtn.clicked()
        }

        // mensagem do backend (erros/infos)
        Label {
            id: feedback
            text: ""
            wrapMode: Text.WordWrap
            Accessible.role: Accessible.StaticText
            Accessible.name: text.length ? text : qsTr("Sem mensagens.")
            Accessible.description: qsTr("Mensagens do sistema e resultados.")
            Accessible.liveRegion: Accessible.Assertive
            Layout.fillWidth: true
        }

        Connections {
            target: backend
            function onErrorMessage(message) { feedback.text = message }
            function onInfoMessage(message)  { feedback.text = message }
        }

        RowLayout {
            spacing: 12
            Layout.alignment: Qt.AlignHCenter

            Button {
                id: loginBtn
                text: qsTr("Entrar")
                enabled: passField.text.length > 0
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Entrar no aplicativo com a senha informada.")
                onClicked: {
                    if (backend.login(root.username, passField.text)) {
                        feedback.text = qsTr("Login realizado.")
                        if (StackView.view) StackView.view.push("show_options.qml")
                    }
                }
                shortcut: StandardKey.Open
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Entrar (atalho: Ctrl+O)")
            }

            Button {
                text: qsTr("Voltar")
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Retornar à tela anterior.")
                onClicked: if (StackView.view) StackView.view.pop()
            }
        }
    }
}
