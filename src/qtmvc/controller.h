#pragma once

#include <QObject>
#include <QString>

// QMVC
#include "password_list_model.h"

// Domínio (model/repositories/services)
#include <password_db/models/model.h>
#include <password_db/repositories/user_repository.h>
#include <password_db/repositories/repository.h>
#include <password_db/services/auth_service.h>
#include <password_db/services/password_service.h>

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(PasswordListModel* passwords READ passwords CONSTANT)

public:
    explicit Controller(QObject* parent = nullptr);

    // Exposto ao QML como model para ListView/TableView
    PasswordListModel* passwords() { return &m_pwList; }

    // Auth
    Q_INVOKABLE bool createUser(const QString& username, const QString& masterPassword);
    Q_INVOKABLE bool login(const QString& username, const QString& masterPassword);

    // Passwords
    Q_INVOKABLE bool     saveManual(const QString& name, const QString& secret);
    Q_INVOKABLE QString  generateAndSave(const QString& name);  // retorna a senha gerada
    Q_INVOKABLE QString  fetchPlain(const QString& name);       // retorna a senha em claro
    Q_INVOKABLE bool     removeByName(const QString& name);
    Q_INVOKABLE void     filterList(const QString& pattern);
    Q_INVOKABLE void     logout();

signals:
    void errorMessage(const QString& message);
    void infoMessage(const QString& message);

private:
    bool checkSession() const;

    // === Ordem importa! Preciso construir na sequência das dependências ===
    PasswordModel      m_model;     // abre DB
    UserRepository     m_userRepo;  // requer m_model.database()
    PasswordRepository m_pwRepo;    // requer m_model.database()
    AuthService        m_auth;      // requer m_userRepo
    PasswordService    m_pw;        // requer m_pwRepo
    PasswordListModel  m_pwList;    // QAbstractItemModel (parent = this)

    QString m_username;
    QString m_master;
};
