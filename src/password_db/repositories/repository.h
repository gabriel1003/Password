#pragma once
#include <QString>
#include <QByteArray>
#include <QSqlDatabase>
#include <QSqlError>
#include <optional>
#include <QList>

#include <password_db/models/model.h>

// Entidade da tabela passwords
struct PasswordEntity {
    QString    name;        // PK
    QByteArray cipher;      // BLOB
    QByteArray iv;          // BLOB (12 bytes)
    QByteArray salt;        // BLOB (16 bytes por registro)
    QByteArray tag;         // BLOB (16 bytes)
    QString    secret_hash; // TEXT (hex)
};

class PasswordRepository {
public:
    explicit PasswordRepository(const QSqlDatabase& db);

    // CRUD
    bool create(const PasswordEntity& e);         // INSERT
    bool update(const PasswordEntity& e);         // UPDATE por name
    bool upsert(const PasswordEntity& e);         // INSERT OR REPLACE
    std::optional<PasswordEntity> get(const QString& name) const; // SELECT por name
    bool remove(const QString& name);             // DELETE
    QList<QString> listNames() const;             // SELECT name ORDER BY name

    QSqlError lastError() const { return m_lastError; }

private:
    bool ensureOpen() const;

    QSqlDatabase m_db;
    mutable QSqlError m_lastError;
};
