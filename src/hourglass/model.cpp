#include "model.h"
#include "database.h"

QSqlDatabase &Model::getDatabase()
{
  return DatabaseManager::getInstance().getDatabase();
}

Model::Model(QObject *parent)
  : QObject(parent)
{
  newRecord = true;
  modified = false;
}

Model::Model(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : QObject(parent), attributes(attributes), newRecord(newRecord)
{
  if (id() == -1) {
    qDebug() << "Record should have an ID, but doesn't!";
  }
  modified = false;
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

void Model::set(const QString &attributeName, QVariant value)
{
  attributes[attributeName] = value;
  modified = true;
}

void Model::unset(const QString &attributeName)
{
  attributes.remove(attributeName);
  modified = true;
}

int Model::id()
{
  QVariant id = get("id");
  if (id.isNull() || !id.isValid()) {
    return -1;
  }
  return id.toInt();
}

bool Model::isNew()
{
  return newRecord;
}

bool Model::isModified()
{
  return modified;
}

bool Model::isValid()
{
  return validate();
}

bool Model::validate()
{
  return true;
}

bool Model::save(QString tableName)
{
  if (attributes.empty() || !isValid()) {
    return false;
  }

  QString queryString;
  if (newRecord) {
    queryString += "INSERT INTO ";
  }
  else {
    queryString += "UPDATE ";
  }
  queryString += tableName + " ";

  if (newRecord) {
    QStringList columns(attributes.keys());
    queryString += "(" + columns.join(", ") + ")";

    QStringList placeholders;
    for (int i = 0; i < columns.size(); i++) {
      placeholders << QString(":%1").arg(columns[i]);
    }
    queryString += " VALUES(" + placeholders.join(", ") + ")";
  }
  else {
    QList<QString> keys = attributes.keys();
    QStringList assignments;
    for (int i = 0; i < keys.count(); i++) {
      if (keys[i] == "id") {
        continue;
      }
      assignments << QString("%1 = :%1").arg(keys[i]);
    }
    queryString += "SET " + assignments.join(", ") + " ";
    queryString += "WHERE id = :id";
  }

  QSqlDatabase &database = getDatabase();
  QSqlQuery query(database);
  query.prepare(queryString);

  QMapIterator<QString, QVariant> i(attributes);
  while (i.hasNext()) {
    i.next();
    query.bindValue(QString(":%1").arg(i.key()), i.value());
  }

  qDebug() << queryString;
  bool result = query.exec();
  if (newRecord && result) {
    newRecord = false;
    set("id", query.lastInsertId().toInt());
  }
  return result;
}
