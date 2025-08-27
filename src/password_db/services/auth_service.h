#pragma once
#include <QString>
#include <QByteArray>

#include <repositories/user_repository.h>
#include "crypt.h"

// Serviço de autenticação: cria usuário e valida login.
// Usa UserRepository. NÃO mexe em schema.
class AuthService {
public:
    explicit AuthService(UserRepository& repo);

    // Cria usuário novo. Falha se já existir.
    bool createUser(const QString& username, const QString& masterPassword);

    // Valida login (username + masterPassword)
    bool login(const QString& username, const QString& masterPassword);

    // Deriva chave (32B) a partir da master do usuário (útil para proteger access_secret, se quiser)
    QByteArray deriveMasterKey(const QString& username, const QString& masterPassword);

    QString lastErrorText() const { return m_lastErrorText; }

private:
    static QString makeVerifier(const QString& masterPassword, const QByteArray& salt);

    UserRepository& m_repo;
    QString m_lastErrorText;
};

src/password_db/services/auth_service.cpp

#include "auth_service.h"

AuthService::AuthService(UserRepository& repo)
    : m_repo(repo) {}

QString AuthService::makeVerifier(const QString& masterPassword, const QByteArray& salt) {
    const QByteArray key = cr_deriveKey(masterPassword, salt, 32, 100000);
    return cr_sha256_hex(key);
}

bool AuthService::createUser(const QString& username, const QString& masterPassword) {
    m_lastErrorText.clear();
    if (username.isEmpty() || masterPassword.isEmpty()) {
        m_lastErrorText = "username e masterPassword são obrigatórios";
        return false;
    }
    auto existing = m_repo.get(username);
    if (existing.has_value()) {
        m_lastErrorText = "usuário já existe";
        return false;
    }
    UserEntity u;
    u.username     = username;
    u.salt         = cr_randomBytes(16);
    u.verifier     = makeVerifier(masterPassword, u.salt);
    u.access_secret.clear(); // opcional, reservado
    if (!m_repo.create(u)) {
        m_lastErrorText = m_repo.lastError().text();
        return false;
    }
    return true;
}

bool AuthService::login(const QString& username, const QString& masterPassword) {
    m_lastErrorText.clear();
    auto u = m_repo.get(username);
    if (!u.has_value()) {
        m_lastErrorText = "usuário não encontrado";
        return false;
    }
    const QString got = makeVerifier(masterPassword, u->salt);
    return (got == u->verifier);
}

QByteArray AuthService::deriveMasterKey(const QString& username, const QString& masterPassword) {
    auto u = m_repo.get(username);
    if (!u.has_value()) return {};
    return cr_deriveKey(masterPassword, u->salt, 32, 100000);
}

