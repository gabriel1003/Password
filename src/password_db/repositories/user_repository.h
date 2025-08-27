#pragma once
#include <QString>
#include <QByteArray>
#include <QSqlDatabase>
#include <QSqlError>
#include <optional>

#include <password_db/models/model.h>

// Entidade da tabela users
struct UserEntity {
    QString    username;     // PK
    QByteArray salt;         // BLOB
    QString    verifier;     // TEXT (hex)
    QByteArray access_secret; // BLOB (opcional; pode estar vazio)
};

class UserRepository {
public:
    explicit UserRepository(const QSqlDatabase& db);

    bool create(const UserEntity& u);          // INSERT
    bool update(const UserEntity& u);          // UPDATE
    bool upsert(const UserEntity& u);          // INSERT OR REPLACE
    std::optional<UserEntity> get(const QString& username) const; // SELECT
    bool remove(const QString& username);      // DELETE

    QSqlError lastError() const { return m_lastError; }

private:
    bool ensureOpen() const;

    QSqlDatabase m_db;
    mutable QSqlError m_lastError;
};
