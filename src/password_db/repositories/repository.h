#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QList>
#include <QString>
#include <optional>

#include <password_db/models/model.h>


// Repositório com operações CRUD sobre a tabela 'passwords'.
class PasswordRepository {
public:
// Constrói a partir de um QSqlDatabase já inicializado/aberto
explicit PasswordRepository(const QSqlDatabase& db);


// Constrói a partir do modelo (usa model.database())
explicit PasswordRepository(const PasswordModel& model);


// CRUD básico
bool create(const PasswordRow& row); // INSERT
bool upsert(const PasswordRow& row); // INSERT ou UPDATE
std::optional<PasswordRow> getByName(const QString& name) const; // SELECT por chave
bool updateSecret(const QString& name, const QString& newSecret); // UPDATE
bool remove(const QString& name); // DELETE
bool exists(const QString& name) const; // SELECT 1
QList<PasswordRow> listAll() const; // SELECT *


// Último erro ocorrido
QSqlError lastError() const { return m_lastError; }


private:
QSqlDatabase m_db;
mutable QSqlError m_lastError;


bool ensureOpen() const; // abre a conexão se necessário
};
