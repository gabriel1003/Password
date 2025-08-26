#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

struct PasswordRow {
    QString name;
    QString secret;
};

class PasswordModel : public QObject {
    Q_OBJECT
public:
    explicit PasswordModel(QObject* parent = nullptr);
    explicit PasswordModel(const QString& connectionName, QObject* parent = nullptr);
    ~PasswordModel();
    static constexpr const char* kTable     = "passwords";
    static constexpr const char* kColName   = "name";   // PRIMARY KEY
    static constexpr const char* kColSecret = "secret"; // NOT NULL

        // Inicializa/garante que a tabela existe
    bool ensureInitialized();

    // Estado/erros
    bool isOpen() const;
    QSqlError lastError() const { return m_lastError; }

    // Acesso ao banco (para outras classes que implementem CRUD)
    const QSqlDatabase& database() const { return m_db; }

private:
    QSqlDatabase m_db;
    QSqlError m_lastError;

    bool attachOrCreateDefaultConnection(const QString& connectionName);
    bool createTableIfMissing();

};