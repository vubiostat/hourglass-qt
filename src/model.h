#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QMap>
#include <QVariant>

class Model : public QObject
{
  Q_OBJECT

  public:
    static QSqlDatabase &getDatabase();

    Model(QObject *parent = 0);
    Model(QMap<QString, QVariant> &attributes, QObject *parent = 0);
    Model(const Model &other);
    Model &operator=(const Model &other);

    QVariant get(const QString &attributeName);

  private:
    QMap<QString, QVariant> attributes;
};

#endif
