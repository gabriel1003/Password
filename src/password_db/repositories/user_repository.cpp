#include "user_repository.h"
#include <QSqlQuery>
#include <QVariant>

UserRepository::UserRepository(const QSqlDatabase& db)
    : m_db(db) {}

bool UserRepository::ensureOpen() const {
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

bool UserRepository::create(const UserEntity& u) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO %1(%2,%3,%4,%5) VALUES(:username,:salt,:verifier,:asec)")
        .arg(PasswordModel::kUsersTable)
        .arg(PasswordModel::kUserColUsername)
        .arg(PasswordModel::kUserColSalt)
        .arg(PasswordModel::kUserColVerifier)
        .arg(PasswordModel::kUserColAccessSec));
    q.bindValue(":username", u.username);
    q.bindValue(":salt",     u.salt);
    q.bindValue(":verifier", u.verifier);
    q.bindValue(":asec",     u.access_secret.isEmpty() ? QVariant(QVariant::ByteArray) : QVariant(u.access_secret));
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return true;
}

bool UserRepository::update(const UserEntity& u) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "UPDATE %1 SET %3=:salt,%4=:verifier,%5=:asec WHERE %2=:username")
        .arg(PasswordModel::kUsersTable)
        .arg(PasswordModel::kUserColUsername)
        .arg(PasswordModel::kUserColSalt)
        .arg(PasswordModel::kUserColVerifier)
        .arg(PasswordModel::kUserColAccessSec));
    q.bindValue(":username", u.username);
    q.bindValue(":salt",     u.salt);
    q.bindValue(":verifier", u.verifier);
    q.bindValue(":asec",     u.access_secret.isEmpty() ? QVariant(QVariant::ByteArray) : QVariant(u.access_secret));
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return q.numRowsAffected() > 0;
}

bool UserRepository::upsert(const UserEntity& u) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT OR REPLACE INTO %1(%2,%3,%4,%5) VALUES(:username,:salt,:verifier,:asec)")
        .arg(PasswordModel::kUsersTable)
        .arg(PasswordModel::kUserColUsername)
        .arg(PasswordModel::kUserColSalt)
        .arg(PasswordModel::kUserColVerifier)
        .arg(PasswordModel::kUserColAccessSec));
    q.bindValue(":username", u.username);
    q.bindValue(":salt",     u.salt);
    q.bindValue(":verifier", u.verifier);
    q.bindValue(":asec",     u.access_secret.isEmpty() ? QVariant(QVariant::ByteArray) : QVariant(u.access_secret));
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return true;
}

std::optional<UserEntity> UserRepository::get(const QString& username) const {
    if (!ensureOpen()) return std::nullopt;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "SELECT %3,%4,%5 FROM %1 WHERE %2=:username")
        .arg(PasswordModel::kUsersTable)
        .arg(PasswordModel::kUserColUsername)
        .arg(PasswordModel::kUserColSalt)
        .arg(PasswordModel::kUserColVerifier)
        .arg(PasswordModel::kUserColAccessSec));
    q.bindValue(":username", username);
    if (!q.exec()) { m_lastError = q.lastError(); return std::nullopt; }
    if (!q.next()) return std::nullopt;

    UserEntity u;
    u.username      = username;
    u.salt          = q.value(0).toByteArray();
    u.verifier      = q.value(1).toString();
    u.access_secret = q.value(2).toByteArray();
    return u;
}

bool UserRepository::remove(const QString& username) {
    if (!ensureOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "DELETE FROM %1 WHERE %2=:username")
        .arg(PasswordModel::kUsersTable)
        .arg(PasswordModel::kUserColUsername));
    q.bindValue(":username", username);
    if (!q.exec()) { m_lastError = q.lastError(); return false; }
    return q.numRowsAffected() > 0;
}
