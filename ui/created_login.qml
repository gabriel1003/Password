import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Accessibility 1.0

Item {
    id: root
    anchors.fill: parent

    // para simplificar, usamos um username padrão; ajuste se quiser expor um campo
    property string username: "default"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12
        width: Math.min(parent.width * 0.9, 420)

        Label {
            id: title
            text: qsTr("Criar login")
            font.pixelSize: 18
            Accessible.role: Accessible.Heading
            Accessible.name: text
        }

        Label {
            id: masterLabel
            text: qsTr("Senha mestra")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        TextField {
            id: masterField
            placeholderText: qsTr("Digite sua senha mestra")
            echoMode: TextInput.Password
            Layout.fillWidth: true
            activeFocusOnTab: true
            Accessible.role: Accessible.EditableText
            Accessible.name: masterLabel.text
            Accessible.description: qsTr("Campo para digitar a senha mestra.")
            labeledBy: masterLabel
            onAccepted: confirmField.forceActiveFocus()
        }

        Label {
            id: confirmLabel
            text: qsTr("Confirmar senha mestra")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        TextField {
            id: confirmField
            placeholderText: qsTr("Confirme a senha mestra")
            echoMode: TextInput.Password
            Layout.fillWidth: true
            activeFocusOnTab: true
            Accessible.role: Accessible.EditableText
            Accessible.name: confirmLabel.text
            Accessible.description: qsTr("Campo para confirmar a senha mestra.")
            labeledBy: confirmLabel
            onAccepted: createBtn.clicked()
        }

        Label {
            id: mismatch
            visible: confirmField.text.length > 0 && (confirmField.text !== masterField.text)
            color: "#c62828"
            text: qsTr("As senhas não coincidem.")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
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
                id: createBtn
                text: qsTr("Criar")
                enabled: masterField.text.length > 0 && (confirmField.text === masterField.text)
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Criar login com a senha mestra informada.")
                onClicked: {
                    if (backend.createUser(root.username, masterField.text)) {
                        feedback.text = qsTr("Usuário criado com sucesso.")
                        if (StackView.view) StackView.view.push("show_options.qml")
                    }
                }
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

