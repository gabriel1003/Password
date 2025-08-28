import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Accessibility 1.0

Item {
    id: root
    anchors.fill: parent

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 14
        width: Math.min(parent.width * 0.9, 520)

        Label {
            id: title
            text: qsTr("Opções")
            font.pixelSize: 18
            Accessible.role: Accessible.Heading
            Accessible.name: text
        }

        // FILTRO + LISTA (usa backend.passwords)
        Label {
            id: filterLabel
            text: qsTr("Filtrar nomes")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        TextField {
            id: filterField
            placeholderText: qsTr("Digite para filtrar a lista")
            Layout.fillWidth: true
            Accessible.role: Accessible.EditableText
            Accessible.name: filterLabel.text
            Accessible.description: qsTr("Campo de filtro para a lista de senhas.")
            labeledBy: filterLabel
            onTextChanged: backend.filterList(text)
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            clip: true
            model: backend.passwords
            Accessible.role: Accessible.List
            Accessible.name: qsTr("Lista de senhas cadastradas")
            delegate: Item {
                width: list.width; height: 40
                Accessible.role: Accessible.ListItem
                Accessible.name: name

                Row {
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        text: name
                        Accessible.role: Accessible.StaticText
                        Accessible.name: text
                    }
                    Button {
                        text: qsTr("Mostrar")
                        Accessible.role: Accessible.Button
                        Accessible.name: qsTr("Mostrar senha para ") + name
                        onClicked: {
                            const plain = backend.fetchPlain(name)
                            feedback.text = plain && plain.length > 0
                                ? qsTr("Senha de '") + name + "': " + plain
                                : qsTr("Senha não encontrada.")
                        }
                    }
                }
            }
        }

        // NOME (chave)
        Label {
            id: nameLabel
            text: qsTr("Nome da senha")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        TextField {
            id: nameField
            placeholderText: qsTr("Ex.: email, banco, github…")
            Layout.fillWidth: true
            activeFocusOnTab: true
            Accessible.role: Accessible.EditableText
            Accessible.name: nameLabel.text
            Accessible.description: qsTr("Informe um identificador para a senha.")
            labeledBy: nameLabel
        }

        // SENHA manual
        Label {
            id: secretLabel
            text: qsTr("Senha (para salvar manualmente)")
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        TextField {
            id: secretField
            placeholderText: qsTr("Digite a senha ou use 'Gerar senha'")
            echoMode: TextInput.Password
            Layout.fillWidth: true
            activeFocusOnTab: true
            Accessible.role: Accessible.EditableText
            Accessible.name: secretLabel.text
            Accessible.description: qsTr("Campo para digitar a senha manualmente.")
            labeledBy: secretLabel
        }

        GridLayout {
            columns: 2
            columnSpacing: 10
            rowSpacing: 10
            Layout.fillWidth: true

            Button {
                id: generateBtn
                text: qsTr("Gerar senha")
                Layout.fillWidth: true
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Gerar uma senha de 14 caracteres e salvar com o nome informado.")
                enabled: nameField.text.length > 0
                onClicked: {
                    const pwd = backend.generateAndSave(nameField.text)
                    if (pwd && pwd.length > 0) {
                        feedback.text = qsTr("Senha gerada e salva: ") + pwd
                        secretField.text = pwd
                    }
                }
            }

            Button {
                id: saveBtn
                text: qsTr("Salvar manual")
                Layout.fillWidth: true
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Salvar a senha digitada para o nome informado.")
                enabled: nameField.text.length > 0 && secretField.text.length > 0
                onClicked: {
                    if (backend.saveManual(nameField.text, secretField.text))
                        feedback.text = qsTr("Senha salva com sucesso.")
                }
            }

            Button {
                id: fetchBtn
                text: qsTr("Buscar senha")
                Layout.fillWidth: true
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Buscar e mostrar a senha referente ao nome informado.")
                enabled: nameField.text.length > 0
                onClicked: {
                    const plain = backend.fetchPlain(nameField.text)
                    feedback.text = plain && plain.length > 0
                        ? qsTr("Senha encontrada: ") + plain
                        : qsTr("Senha não encontrada.")
                }
            }

            Button {
                id: deleteBtn
                text: qsTr("Deletar senha")
                Layout.fillWidth: true
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Apagar a senha vinculada ao nome informado.")
                enabled: nameField.text.length > 0
                onClicked: {
                    if (backend.removeByName(nameField.text)) {
                        feedback.text = qsTr("Senha deletada.")
                        secretField.clear()
                    }
                }
            }
        }

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
            Layout.alignment: Qt.AlignRight
            Button {
                text: qsTr("Sair")
                activeFocusOnTab: true
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: qsTr("Sair da sessão e retornar à tela inicial.")
                onClicked: {
                    backend.logout()
                    if (StackView.view) StackView.view.pop(null)
                    if (StackView.view) StackView.view.pop(null)
                }
            }
        }
    }
}
