#include "model.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QVariant>
#include <QDebug>

namespace {
static const char* kCreatePasswordsSql =
    "CREATE TABLE IF NOT EXISTS passwords ("
    "  name        TEXT PRIMARY KEY,"
    "  cipher      BLOB NOT NULL,"
    "  iv          BLOB NOT NULL,"
    "  salt        BLOB NOT NULL,"
    "  tag         BLOB NOT NULL,"
    "  secret_hash TEXT NOT NULL"
    ")";

static const char* kCreateUsersSql =
    "CREATE TABLE IF NOT EXISTS users ("
    "  username      TEXT PRIMARY KEY,"
    "  salt          BLOB NOT NULL,"
    "  verifier      TEXT NOT NULL,"
    "  access_secret BLOB NULL"
    ")";
} // namespace

PasswordModel::PasswordModel(QObject* parent)
    : QObject(parent) {
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
        m_db = QSqlDatabase::database(); // conexão padrão, se já existir
    } else {
        // conexão SQLite local padrão
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
    return createOrMigrateSchema();
}

bool PasswordModel::createOrMigrateSchema() {
    if (!createPasswordsTable()) return false;
    if (!createUsersTable())     return false;

    // Para SQLite, checa colunas esperadas
    if (isSQLiteDriver(m_db)) {
        if (!verifyPasswordsColumnsSQLite()) return false;
        if (!verifyUsersColumnsSQLite())     return false;
    }
    return true;
}

bool PasswordModel::createPasswordsTable() {
    QSqlQuery q(m_db);
    if (!q.exec(QString::fromUtf8(kCreatePasswordsSql))) {
        m_lastError = q.lastError();
        return false;
    }
    return true;
}

bool PasswordModel::createUsersTable() {
    QSqlQuery q(m_db);
    if (!q.exec(QString::fromUtf8(kCreateUsersSql))) {
        m_lastError = q.lastError();
        return false;
    }
    return true;
}

bool PasswordModel::isSQLiteDriver(const QSqlDatabase& db) {
    return db.driverName().compare(QStringLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
}

bool PasswordModel::verifyPasswordsColumnsSQLite() {
    QSqlQuery q(m_db);
    if (!q.exec(QStringLiteral("PRAGMA table_info(%1)").arg(QString::fromUtf8(kPasswordsTable)))) {
        m_lastError = q.lastError();
        return false;
    }

    bool hasName=false, hasCipher=false, hasIv=false, hasSalt=false, hasTag=false, hasHash=false;
    while (q.next()) {
        const QString col = q.value(1).toString(); // coluna "name" do PRAGMA
        if      (col == QLatin1String(kPwColName))       hasName  = true;
        else if (col == QLatin1String(kPwColCipher))     hasCipher= true;
        else if (col == QLatin1String(kPwColIv))         hasIv    = true;
        else if (col == QLatin1String(kPwColSalt))       hasSalt  = true;
        else if (col == QLatin1String(kPwColTag))        hasTag   = true;
        else if (col == QLatin1String(kPwColSecretHash)) hasHash  = true;
    }

    if (!(hasName && hasCipher && hasIv && hasSalt && hasTag && hasHash)) {
        m_lastError = QSqlError("Unexpected schema for 'passwords' table", QString(), QSqlError::StatementError);
        return false;
    }
    return true;
}

bool PasswordModel::verifyUsersColumnsSQLite() {
    QSqlQuery q(m_db);
    if (!q.exec(QStringLiteral("PRAGMA table_info(%1)").arg(QString::fromUtf8(kUsersTable)))) {
        m_lastError = q.lastError();
        return false;
    }

    bool hasUser=false, hasSalt=false, hasVerifier=false, hasAccess=false;
    while (q.next()) {
        const QString col = q.value(1).toString();
        if      (col == QLatin1String(kUserColUsername))  hasUser    = true;
        else if (col == QLatin1String(kUserColSalt))      hasSalt    = true;
        else if (col == QLatin1String(kUserColVerifier))  hasVerifier= true;
        else if (col == QLatin1String(kUserColAccessSec)) hasAccess  = true;
    }

    if (!(hasUser && hasSalt && hasVerifier && hasAccess)) {
        m_lastError = QSqlError("Unexpected schema for 'users' table", QString(), QSqlError::StatementError);
        return false;
    }
    return true;
}