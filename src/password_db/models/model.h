#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

class PasswordModel : public QObject {
    Q_OBJECT
public:
    explicit PasswordModel(QObject* parent = nullptr);
    explicit PasswordModel(const QString& connectionName, QObject* parent = nullptr);
    ~PasswordModel();

    // Tabela de senhas (armazenamento criptografado por registro)
    static constexpr const char* kPasswordsTable   = "passwords";
    static constexpr const char* kPwColName        = "name";
    static constexpr const char* kPwColCipher      = "cipher";
    static constexpr const char* kPwColIv          = "iv";
    static constexpr const char* kPwColSalt        = "salt";
    static constexpr const char* kPwColTag         = "tag";
    static constexpr const char* kPwColSecretHash  = "secret_hash";

    // Tabela de usuários (login / master password verificada por hash)
    static constexpr const char* kUsersTable       = "users";
    static constexpr const char* kUserColUsername  = "username";
    static constexpr const char* kUserColSalt      = "salt";         // BLOB (PBKDF2 salt)
    static constexpr const char* kUserColVerifier  = "verifier";     // HEX(SHA256(PBKDF2(master, salt)))
    static constexpr const char* kUserColAccessSec = "access_secret";// BLOB NULL (reservado/uso pelos services)

    // Abre conexão e garante o esquema
    bool ensureInitialized();

    // Estado/erros
    bool isOpen() const;
    QSqlError lastError() const { return m_lastError; }

    // Acesso ao QSqlDatabase subjacente
    const QSqlDatabase& database() const { return m_db; }

private:
    QSqlDatabase m_db;
    QSqlError m_lastError;

    bool attachOrCreateDefaultConnection(const QString& connectionName);
    bool createOrMigrateSchema();

    // criação das tabelas
    bool createPasswordsTable();
    bool createUsersTable();

    // verificações (SQLite)
    bool verifyPasswordsColumnsSQLite();
    bool verifyUsersColumnsSQLite();
    static bool isSQLiteDriver(const QSqlDatabase& db);
};
