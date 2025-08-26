#include "model.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QVariant>

namespace {
// SQL de criação da tabela
static const char* kCreateTableSql =
    "CREATE TABLE IF NOT EXISTS passwords ("
    "  name   TEXT PRIMARY KEY,"
    "  secret TEXT NOT NULL"
    ")";
}

PasswordModel::PasswordModel(QObject* parent)
    : QObject(parent) {
    // Usa a conexão padrão ou cria uma SQLite local
    attachOrCreateDefaultConnection(QString());
}

PasswordModel::PasswordModel(const QString& connectionName, QObject* parent)
    : QObject(parent) {
    attachOrCreateDefaultConnection(connectionName);
}

PasswordModel::~PasswordModel() = default;

bool PasswordModel::attachOrCreateDefaultConnection(const QString& connectionName) {
    m_lastError = QSqlError();

    if (!connectionName.isEmpty() && QSqlDatabase::contains(connectionName)) {
        m_db = QSqlDatabase::database(connectionName);
    } else if (QSqlDatabase::contains()) {
        m_db = QSqlDatabase::database(); // conexão padrão existente
    } else {
        // cria conexão SQLite local
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("passwords.db");
    }

    if (!m_db.isValid()) {
        m_lastError = QSqlError("Invalid QSqlDatabase", QString(), QSqlError::ConnectionError);
        return false;
    }

    return true;
}

bool PasswordModel::isOpen() const {
    return m_db.isValid() && m_db.isOpen();
}

bool PasswordModel::ensureInitialized() {
    m_lastError = QSqlError();

    if (!m_db.isValid()) {
        m_lastError = QSqlError("Invalid QSqlDatabase", QString(), QSqlError::ConnectionError);
        return false;
    }

    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            m_lastError = m_db.lastError();
            return false;
        }
    }

    return createTableIfMissing();
}

bool PasswordModel::createTableIfMissing() {
    QSqlQuery q(m_db);
    if (!q.exec(QString::fromUtf8(kCreateTableSql))) {
        m_lastError = q.lastError();
        return false;
    }

    // opcional: verificar colunas
    if (!q.exec(QStringLiteral("PRAGMA table_info(%1)").arg(QString::fromUtf8(kTable)))) {
        m_lastError = q.lastError();
        return false;
    }

    bool hasName = false, hasSecret = false;
    while (q.next()) {
        const QString col = q.value(1).toString();
        if (col == QLatin1String(kColName))   hasName = true;
        if (col == QLatin1String(kColSecret)) hasSecret = true;
    }

    if (!hasName || !hasSecret) {
        m_lastError = QSqlError("Unexpected schema for 'passwords' table", QString(), QSqlError::StatementError);
        return false;
    }

    return true;
}