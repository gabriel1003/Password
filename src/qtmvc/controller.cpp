#include "controller.h"
#include <QDebug>

// sua função geradora (retorna std::string de 14 caracteres)
#include <generate_password/generator_password.h>

Controller::Controller(QObject* parent)
    : QObject(parent)
    // constrói dependências em ordem:
    , m_model(this)                                // PasswordModel é QObject? (se sim, parent = this)
    , m_userRepo(m_model.database())
    , m_pwRepo(m_model.database())
    , m_auth(m_userRepo)
    , m_pw(m_pwRepo)
    , m_pwList(this)                               // model da lista com parent para hierarquia de QObject
{
    if (!m_model.ensureInitialized()) {
        qWarning() << "DB init error:" << m_model.lastError().text();
    }
    m_pwList.refresh();
}

// =================== Auth ===================
bool Controller::createUser(const QString& username, const QString& masterPassword) {
    if (username.isEmpty() || masterPassword.isEmpty()) {
        emit errorMessage(tr("Usuário e senha são obrigatórios."));
        return false;
    }
    const bool ok = m_auth.createUser(username, masterPassword);
    if (!ok) {
        emit errorMessage(m_auth.lastErrorText());
        return false;
    }
    m_username = username;
    m_master   = masterPassword;
    emit infoMessage(tr("Usuário criado com sucesso."));
    return true;
}

bool Controller::login(const QString& username, const QString& masterPassword) {
    if (username.isEmpty() || masterPassword.isEmpty()) {
        emit errorMessage(tr("Usuário e senha são obrigatórios."));
        return false;
    }
    const bool ok = m_auth.login(username, masterPassword);
    if (!ok) {
        emit errorMessage(tr("Login inválido."));
        return false;
    }
    m_username = username;
    m_master   = masterPassword;
    emit infoMessage(tr("Login realizado."));
    return true;
}

// =============== Passwords (CRUD seguro) ===============
bool Controller::saveManual(const QString& name, const QString& secret) {
    if (!checkSession()) return false;
    if (name.isEmpty() || secret.isEmpty()) {
        emit errorMessage(tr("Informe nome e senha."));
        return false;
    }
    const bool ok = m_pw.save(m_master, name, secret);
    if (!ok) emit errorMessage(tr("Falha ao salvar."));
    m_pwList.refresh();
    return ok;
}

QString Controller::generateAndSave(const QString& name) {
    if (!checkSession()) return {};
    if (name.isEmpty()) {
        emit errorMessage(tr("Informe um nome."));
        return {};
    }
    const auto gen = geneerator_password();   // std::string (14 chars)
    const QString pwd = QString::fromStdString(gen);
    if (pwd.isEmpty()) {
        emit errorMessage(tr("Falha ao gerar senha."));
        return {};
    }
    if (!m_pw.save(m_master, name, pwd)) {
        emit errorMessage(tr("Falha ao salvar senha gerada."));
        return {};
    }
    m_pwList.refresh();
    return pwd;
}

QString Controller::fetchPlain(const QString& name) {
    if (!checkSession()) return {};
    if (name.isEmpty()) {
        emit errorMessage(tr("Informe um nome."));
        return {};
    }
    auto plain = m_pw.fetch(m_master, name);
    if (!plain.has_value()) {
        emit errorMessage(tr("Senha não encontrada ou master inválida."));
        return {};
    }
    return *plain;
}

bool Controller::removeByName(const QString& name) {
    if (!checkSession()) return false;
    if (name.isEmpty()) {
        emit errorMessage(tr("Informe um nome."));
        return false;
    }
    const bool ok = m_pw.remove(name);
    if (!ok) emit errorMessage(tr("Falha ao deletar."));
    m_pwList.refresh();
    return ok;
}

void Controller::filterList(const QString& pattern) {
    m_pwList.setFilter(pattern);
}

void Controller::logout() {
    m_username.clear();
    m_master.clear();
    emit infoMessage(tr("Sessão encerrada."));
}

bool Controller::checkSession() const {
    if (m_username.isEmpty() || m_master.isEmpty()) {
        emit errorMessage(tr("Faça login primeiro."));
        return false;
    }
    return true;
}