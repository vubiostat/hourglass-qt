#include <QtDebug>
#include "model.h"
#include "database.h"

QSqlDatabase &Model::getDatabase()
{
  return DatabaseManager::getInstance().getDatabase();
}

Model::Model(QObject *parent)
  : QObject(parent)
{
  empty = true;
}

Model::Model(QMap<QString, QVariant> &attributes, QObject *parent)
  : QObject(parent), attributes(attributes)
{
  if (id() == -1) {
    qDebug() << "Record should have an ID, but doesn't!";
  }
  empty = false;
}

Model::Model(const Model &other)
  : QObject(other.parent())
{
  attributes = other.attributes;
}

Model &Model::operator=(const Model &other)
{
  attributes = other.attributes;
}

QVariant Model::get(const QString &attributeName)
{
  return attributes[attributeName];
}

int Model::id()
{
  QVariant id = get("id");
  if (id.isNull() || !id.isValid()) {
    return -1;
  }
  return id.toInt();
}

bool Model::isEmpty()
{
  return empty;
}
