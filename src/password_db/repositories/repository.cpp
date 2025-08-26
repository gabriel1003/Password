#include "repository.h"


#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>


PasswordRepository::PasswordRepository(const QSqlDatabase& db)
: m_db(db) {}


PasswordRepository::PasswordRepository(const PasswordModel& model)
: m_db(model.database()) {}


bool PasswordRepository::ensureOpen() const {
m_lastError = QSqlError();
if (!m_db.isValid()) {
m_lastError = QSqlError("Invalid QSqlDatabase", QString(), QSqlError::ConnectionError);
return false;
}
if (!m_db.isOpen()) {
QSqlDatabase copy = m_db; // QSqlDatabase é um handle/copiar reabre a mesma conexão
if (!copy.open()) {
m_lastError = copy.lastError();
return false;
}
}
return true;
}


bool PasswordRepository::create(const PasswordRow& row) {
if (!ensureOpen()) return false;


QSqlQuery q(m_db);
q.prepare(QStringLiteral(
"INSERT INTO %1(%2, %3) VALUES (:name, :secret)")
.arg(QString::fromUtf8(PasswordModel::kTable))
.arg(QString::fromUtf8(PasswordModel::kColName))
.arg(QString::fromUtf8(PasswordModel::kColSecret))
);
q.bindValue(":name", row.name);
q.bindValue(":secret", row.secret);


if (!q.exec()) {
m_lastError = q.lastError();
return false;
}
return true;
}


bool PasswordRepository::upsert(const PasswordRow& row) {
// Estratégia portável: tenta UPDATE; se zero linhas afetadas, faz INSERT.
if (!ensureOpen()) return false;


if (updateSecret(row.name, row.secret)) {
return true; // Atualizou com sucesso
}


// Se o erro do update foi "no such table" ou algo grave, já retorna
if (m_lastError.isValid() && m_lastError.type() == QSqlError::StatementError) {
// deixa o erro como está
return false;
}


// Caso não tenha atualizado (p.ex. não existia), tenta inserir
return create(row);
}


std::optional<PasswordRow> PasswordRepository::getByName(const QString& name) const {
if (!ensureOpen()) return std::nullopt;


QSqlQuery q(m_db);
q.prepare(QStringLiteral(
"SELECT %2, %3 FROM %1 WHERE %2 = :name")
.arg(QString::fromUtf8(PasswordModel::kTable))
.arg(QString::fromUtf8(PasswordModel::kColName))
.arg(QString::fromUtf8(PasswordModel::kColSecret))
);
q.bindValue(":name", name);


if (!q.exec()) {
m_lastError = q.lastError();
return std::nullopt;
}
if (!q.next()) {
return std::nullopt; // não encontrado
}


PasswordRow row;
row.name = q.value(0).toString();
row.secret = q.value(1).toString();
return row;
}


bool PasswordRepository::updateSecret(const QString& name, const QString& newSecret) {
if (!ensureOpen()) return false;


QSqlQuery q(m_db);
q.prepare(QStringLiteral(
"UPDATE %1 SET %3 = :secret WHERE %2 = :name")
.arg(QString::fromUtf8(PasswordModel::kTable))
.arg(QString::fromUtf8(PasswordModel::kColName))
.arg(QString::fromUtf8(PasswordModel::kColSecret))
);
q.bindValue(":name", name);
q.bindValue(":secret", newSecret);


if (!q.exec()) {
m_lastError = q.lastError();
return false;
}


return q.numRowsAffected()> 0; // true se atualizou alguma linha
}


bool PasswordRepository::remove(const QString& name) {
if (!ensureOpen()) return false;


QSqlQuery q(m_db);
q.prepare(QStringLiteral(
"DELETE FROM %1 WHERE %2 = :name")
.arg(QString::fromUtf8(PasswordModel::kTable))
.arg(QString::fromUtf8(PasswordModel::kColName))
);
q.bindValue(":name", name);


if (!q.exec()) {
m_lastError = q.lastError();
return false;
}
return q.numRowsAffected() > 0;
}


bool PasswordRepository::exists(const QString& name) const {
if (!ensureOpen()) return false;


QSqlQuery q(m_db);
q.prepare(QStringLiteral(
"SELECT 1 FROM %1 WHERE %2 = :name LIMIT 1")
.arg(QString::fromUtf8(PasswordModel::kTable))
.arg(QString::fromUtf8(PasswordModel::kColName))
);
q.bindValue(":name", name);


if (!q.exec()) {
m_lastError = q.lastError();
return false;
}
return q.next();
}


QList<PasswordRow> PasswordRepository::listAll() const {
QList<PasswordRow> out;
if (!ensureOpen()) return out;


QSqlQuery q(m_db);
if (!q.exec(QStringLiteral(
"SELECT %2, %3 FROM %1 ORDER BY %2 ASC")
        .arg(QString::fromUtf8(PasswordModel::kTable))
        .arg(QString::fromUtf8(PasswordModel::kColName))
        .arg(QString::fromUtf8(PasswordModel::kColSecret))
    )) {
        m_lastError = q.lastError();
        return out;
    }


    while (q.next()) {
        PasswordRow row;
        row.name = q.value(0).toString();
        row.secret = q.value(1).toString();
        out.append(row);
    }
    return out;
}

