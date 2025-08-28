#include <QSqlQueryModel>

class PasswordListModel : public QSqlQueryModel {
    Q_OBJECT
public:
    enum Roles { NameRole = Qt::UserRole + 1 };
    explicit PasswordListModel(QObject* parent=nullptr);

    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& idx, int role) const override;

public slots:
    void refresh();              // roda SELECT name FROM passwords ORDER BY name
    void setFilter(const QString& pattern); // opcional: filtra por LIKE

private:
    QString m_filter;
};
