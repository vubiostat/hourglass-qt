#include "model.h"
#include "database.h"

QSqlDatabase &Model::getDatabase()
{
  return DatabaseManager::getInstance().getDatabase();
}

Model::Model(QObject *parent)
  : QObject(parent)
{
}

Model::Model(QMap<QString, QVariant> &attributes, QObject *parent)
  : QObject(parent), attributes(attributes)
{
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

