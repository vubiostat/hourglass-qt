#include "record.h"
#include <QDateTime>

QSqlDatabase Record::database()
{
  return QSqlDatabase::database();
}

int Record::count(QString tableName)
{
  return count(tableName, QString());
}

int Record::count(QString tableName, QString conditions)
{
  QList<QVariant> emptyBindValues;
  return count(tableName, conditions, emptyBindValues);
}

int Record::count(QString tableName, QString conditions, const QList<QVariant> &bindValues)
{
  QStringList queryStrings;
  queryStrings << "SELECT COUNT(*) FROM";
  queryStrings << tableName;
  if (!conditions.isEmpty()) {
    queryStrings << conditions;
  }
  QString queryString = queryStrings.join(" ");

  QSqlDatabase database = Record::database();
  QSqlQuery query(database);
  query.prepare(queryString);
  for (int i = 0; i < bindValues.size(); i++) {
    query.addBindValue(bindValues[i]);
  }
  if (!query.exec()) {
    qDebug() << "Query failed:" << queryString;
    return -1;
  }

  if (query.next()) {
    return query.value(0).toInt();
  }
  return -1;
}

Record::Record(QObject *parent)
  : QObject(parent)
{
  newRecord = true;
  modified = false;
}

Record::Record(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : QObject(parent), attributes(attributes), newRecord(newRecord)
{
  if (id() == -1) {
    qDebug() << "Record should have an ID, but doesn't!";
  }
  newRecord = false;
  modified = false;
}

Record::Record(const Record &other)
  : QObject(other.parent())
{
  newRecord = other.isNew();
  modified = other.isModified();
  attributes = other.attributes;
}

Record &Record::operator=(const Record &other)
{
  newRecord = other.isNew();
  modified = other.isModified();
  attributes = other.attributes;
  return *this;
}

QVariant Record::get(const QString &attributeName) const
{
  return attributes[attributeName];
}

void Record::set(const QString &attributeName, QVariant value)
{
  attributes[attributeName] = value;
  modified = true;
}

void Record::unset(const QString &attributeName)
{
  attributes.remove(attributeName);
  modified = true;
}

int Record::id() const
{
  QVariant id = get("id");
  if (id.isNull() || !id.isValid()) {
    return -1;
  }
  return id.toInt();
}

bool Record::isNew() const
{
  return newRecord;
}

bool Record::isModified() const
{
  return modified;
}

bool Record::isValid()
{
  beforeValidation();
  return validate();
}

void Record::beforeValidation()
{
}

bool Record::validate()
{
  return true;
}

bool Record::save(QString tableName)
{
  if (attributes.empty() || !isValid()) {
    return false;
  }

  if (newRecord) {
    beforeCreate();
  }
  beforeSave();

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

  QSqlDatabase database = Record::database();
  QSqlQuery query(database);
  query.prepare(queryString);

  QMapIterator<QString, QVariant> i(attributes);
  while (i.hasNext()) {
    i.next();
    query.bindValue(QString(":%1").arg(i.key()), i.value());
  }

  //qDebug() << queryString;
  bool result = query.exec();
  if (newRecord && result) {
    newRecord = false;
    set("id", query.lastInsertId().toInt());
    afterCreate();
  }
  afterSave();
  return result;
}

void Record::beforeCreate()
{
  set("created_at", QDateTime::currentDateTime());
}

void Record::beforeSave()
{
  set("updated_at", QDateTime::currentDateTime());
}

void Record::afterSave()
{
}

void Record::afterCreate()
{
}

bool Record::destroy(QString tableName)
{
  if (newRecord) {
    return false;
  }

  QString queryString("DELETE FROM " + tableName + " WHERE id = ?");

  QSqlDatabase database = Record::database();
  QSqlQuery query(database);
  query.prepare(queryString);
  query.addBindValue(id());

  //qDebug() << queryString;
  return query.exec();
}

bool Record::operator==(const Record &other)
{
  return !isNew() && !other.isNew() && id() == other.id();
}
