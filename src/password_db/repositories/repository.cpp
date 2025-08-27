#include "password_repository.h"
#include <QSqlQuery>
#include <QVariant>

PasswordRepository::PasswordRepository(const QSqlDatabase& db)
    : m_db(db) {}

bool PasswordRepository::ensureOpen() const {
    m_lastError = QSqlError();
    if (!m_db.isValid()) {
        m_lastError = QSqlError("Invalid QSqlDatabase", {}, QSqlError::ConnectionError);
        return false;
    }
    if (!m_db.isOpen()) {
        QSqlDatabase copy = m_db;
        if (!copy.open()) {
            m_lastError = copy.lastError();
            return false;
        }
    }
    return true;
}

bool PasswordRepository::create(const PasswordEntity& e) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO %1(%2,%3,%4,%5,%6,%7) "
        "VALUES(:name,:cipher,:iv,:salt,:tag,:hash)")
        .arg(PasswordModel::kPasswordsTable)
        .arg(PasswordModel::kPwColName)
        .arg(PasswordModel::kPwColCipher)
        .arg(PasswordModel::kPwColIv)
        .arg(PasswordModel::kPwColSalt)
        .arg(PasswordModel::kPwColTag)
        .arg(PasswordModel::kPwColSecretHash));
    q.bindValue(":name",   e.name);
    q.bindValue(":cipher", e.cipher);
    q.bindValue(":iv",     e.iv);
    q.bindValue(":salt",   e.salt);
    q.bindValue(":tag",    e.tag);
    q.bindValue(":hash",   e.secret_hash);
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return true;
}

bool PasswordRepository::update(const PasswordEntity& e) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "UPDATE %1 SET %3=:cipher,%4=:iv,%5=:salt,%6=:tag,%7=:hash WHERE %2=:name")
        .arg(PasswordModel::kPasswordsTable)
        .arg(PasswordModel::kPwColName)
        .arg(PasswordModel::kPwColCipher)
        .arg(PasswordModel::kPwColIv)
        .arg(PasswordModel::kPwColSalt)
        .arg(PasswordModel::kPwColTag)
        .arg(PasswordModel::kPwColSecretHash));
    q.bindValue(":name",   e.name);
    q.bindValue(":cipher", e.cipher);
    q.bindValue(":iv",     e.iv);
    q.bindValue(":salt",   e.salt);
    q.bindValue(":tag",    e.tag);
    q.bindValue(":hash",   e.secret_hash);
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return q.numRowsAffected() > 0;
}

bool PasswordRepository::upsert(const PasswordEntity& e) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT OR REPLACE INTO %1(%2,%3,%4,%5,%6,%7) "
        "VALUES(:name,:cipher,:iv,:salt,:tag,:hash)")
        .arg(PasswordModel::kPasswordsTable)
        .arg(PasswordModel::kPwColName)
        .arg(PasswordModel::kPwColCipher)
        .arg(PasswordModel::kPwColIv)
        .arg(PasswordModel::kPwColSalt)
        .arg(PasswordModel::kPwColTag)
        .arg(PasswordModel::kPwColSecretHash));
    q.bindValue(":name",   e.name);
    q.bindValue(":cipher", e.cipher);
    q.bindValue(":iv",     e.iv);
    q.bindValue(":salt",   e.salt);
    q.bindValue(":tag",    e.tag);
    q.bindValue(":hash",   e.secret_hash);
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return true;
}

std::optional<PasswordEntity> PasswordRepository::get(const QString& name) const {
    if (!ensureOpen()) return std::nullopt;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "SELECT %3,%4,%5,%6,%7 FROM %1 WHERE %2 = :name")
        .arg(PasswordModel::kPasswordsTable)
        .arg(PasswordModel::kPwColName)
        .arg(PasswordModel::kPwColCipher)
        .arg(PasswordModel::kPwColIv)
        .arg(PasswordModel::kPwColSalt)
        .arg(PasswordModel::kPwColTag)
        .arg(PasswordModel::kPwColSecretHash));
    q.bindValue(":name", name);
    if (!q.exec()) { m_lastError = q.lastError(); return std::nullopt; }
    if (!q.next()) return std::nullopt;

    PasswordEntity e;
    e.name        = name;
    e.cipher      = q.value(0).toByteArray();
    e.iv          = q.value(1).toByteArray();
    e.salt        = q.value(2).toByteArray();
    e.tag         = q.value(3).toByteArray();
    e.secret_hash = q.value(4).toString();
    return e;
}

bool PasswordRepository::remove(const QString& name) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "DELETE FROM %1 WHERE %2 = :name")
        .arg(PasswordModel::kPasswordsTable)
        .arg(PasswordModel::kPwColName));
    q.bindValue(":name", name);
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return q.numRowsAffected() > 0;
}

QList<QString> PasswordRepository::listNames() const {
    QList<QString> out;
    if (!ensureOpen()) return out;
    QSqlQuery q(m_db);
    if (!q.exec(QStringLiteral(
            "SELECT %2 FROM %1 ORDER BY %2 ASC")
            .arg(PasswordModel::kPasswordsTable)
            .arg(PasswordModel::kPwColName))) {
        m_lastError = q.lastError();
        return out;
    }
    while (q.next()) out.append(q.value(0).toString());
    return out;
}
