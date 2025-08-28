#include "password_list_model.h"
#include <QSqlQuery>
#include <QSqlDatabase>

PasswordListModel::PasswordListModel(QObject* p) : QSqlQueryModel(p) {}

QHash<int,QByteArray> PasswordListModel::roleNames() const {
    return {{ NameRole, "name" }};  // papel legível no QML
}

QVariant PasswordListModel::data(const QModelIndex& idx, int role) const {
    if (!idx.isValid()) return {};
    switch (role) {
    case NameRole: return QSqlQueryModel::data(this->index(idx.row(), 0), Qt::DisplayRole);
    default:       return {};
    }
}

void PasswordListModel::refresh() {
    QString sql = "SELECT name FROM passwords";
    if (!m_filter.isEmpty())
        sql += " WHERE name LIKE :f";
    sql += " ORDER BY name ASC";

    QSqlQuery q(QSqlDatabase::database());
    q.prepare(sql);
    if (!m_filter.isEmpty())
        q.bindValue(":f", "%" + m_filter + "%");
    q.exec();
    setQuery(q);   // QSqlQueryModel::setQuery(QSqlQuery) com bind seguro
}

void PasswordListModel::setFilter(const QString& pattern) {
    m_filter = pattern;
    refresh();
}
