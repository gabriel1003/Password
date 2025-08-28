#pragma once

#include <QObject>
#include <QString>
#include <memory>

#include "password_list_model.h"
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
    PasswordListModel* passwords();

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

    std::unique_ptr<PasswordModel>       m_model;
    std::unique_ptr<UserRepository>      m_userRepo;
    std::unique_ptr<PasswordRepository>  m_pwRepo;
    std::unique_ptr<AuthService>         m_auth;
    std::unique_ptr<PasswordService>     m_pw;
    std::unique_ptr<PasswordListModel>   m_pwList;

    QString m_username;
    QString m_master;
};