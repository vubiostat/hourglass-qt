#include "hourglass/record.h"
#include <QDateTime>
#include <QSqlError>

QSqlDatabase Record::s_database;

QSqlDatabase &Record::database()
{
  return s_database;
}

void Record::setDatabase(QSqlDatabase &database)
{
  s_database = database;
}

QList<int> Record::findIds(const QString &tableName)
{
  return findIds(tableName, QString());
}

QList<int> Record::findIds(const QString &tableName, const QString &conditions)
{
  QList<QVariant> emptyBindValues;
  return findIds(tableName, conditions, emptyBindValues);
}

QList<int> Record::findIds(const QString &tableName, const QString &conditions, const QString &predicate)
{
  QList<QVariant> emptyBindValues;
  return findIds(tableName, conditions, emptyBindValues, predicate);
}

QList<int> Record::findIds(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues)
{
  return findIds(tableName, conditions, bindValues, QString());
}

QList<int> Record::findIds(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate)
{
  QSqlQuery query;
  if (!executeFindQuery(query, tableName, "id", conditions, bindValues, predicate)) {
    qDebug() << "Query failed:" << query.lastQuery();
    return QList<int>();
  }

  QList<int> result;
  while (query.next()) {
    result.append(query.value(0).toInt());
  }
  return result;
}

int Record::count(const QString &tableName)
{
  return count(tableName, QString());
}

int Record::count(const QString &tableName, const QString &conditions)
{
  QList<QVariant> emptyBindValues;
  return count(tableName, conditions, emptyBindValues);
}

int Record::count(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues)
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
  newRecord = wasNewRecord = true;
  modified = false;
}

Record::Record(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : QObject(parent), attributes(attributes), newRecord(newRecord), wasNewRecord(newRecord)
{
  if (id() == -1) {
    qDebug() << "Record should have an ID, but doesn't!";
  }
  modified = false;
}

Record::Record(const Record &other)
  : QObject(other.parent())
{
  newRecord = other.newRecord;
  wasNewRecord = other.wasNewRecord;
  modified = other.modified;
  attributes = other.attributes;
  dirtyAttributes = other.dirtyAttributes;
}

Record &Record::operator=(const Record &other)
{
  newRecord = other.newRecord;
  wasNewRecord = other.wasNewRecord;
  modified = other.modified;
  attributes = other.attributes;
  dirtyAttributes = other.dirtyAttributes;
  return *this;
}

QVariant Record::get(const QString &attributeName, bool dirty) const
{
  if (dirty && dirtyAttributes.contains(attributeName)) {
    return dirtyAttributes[attributeName];
  }
  else {
    return attributes[attributeName];
  }
}

void Record::set(const QString &attributeName, const QVariant &value)
{
  dirtyAttributes[attributeName] = value;
  modified = true;
}

void Record::unset(const QString &attributeName)
{
  if (dirtyAttributes.contains(attributeName)) {
    dirtyAttributes.remove(attributeName);
  }
  else {
    dirtyAttributes[attributeName] = QVariant();
  }
  modified = true;
}

bool Record::containsAttribute(const QString &attributeName) const
{
  return dirtyAttributes.contains(attributeName) ||
    attributes.contains(attributeName);
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

bool Record::wasNew() const
{
  return wasNewRecord;
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

bool Record::save(const QString &tableName)
{
  if (!isValid()) {
    return false;
  }

  if (newRecord) {
    beforeCreate();
  }
  beforeSave();

  QString queryString;
  if (newRecord) {
    queryString.append("INSERT INTO ");
  }
  else {
    queryString.append("UPDATE ");
  }
  queryString.append(tableName + " ");

  QStringList keys = dirtyAttributes.keys();
  if (newRecord) {
    queryString.append("(" + keys.join(", ") + ")");

    QStringList placeholders;
    for (int i = 0; i < keys.size(); i++) {
      placeholders << QString(":%1").arg(keys[i]);
    }
    queryString.append(" VALUES(" + placeholders.join(", ") + ")");
  }
  else {
    QStringList assignments;
    for (int i = 0; i < keys.count(); i++) {
      if (keys[i] == "id") {
        continue;
      }
      assignments << QString("%1 = :%1").arg(keys[i]);
    }
    queryString.append("SET " + assignments.join(", ") + " ");
    queryString.append("WHERE id = :id");
  }

  QSqlDatabase database = Record::database();
  QSqlQuery query(database);
  query.prepare(queryString);

  QMapIterator<QString, QVariant> i(dirtyAttributes);
  while (i.hasNext()) {
    i.next();
    query.bindValue(QString(":%1").arg(i.key()), i.value());
  }
  if (!newRecord) {
    query.bindValue(":id", id());
  }

  //qDebug() << queryString;
  bool result = query.exec();
  if (result) {
    if (newRecord) {
      newRecord = false;

      /* Not using set(), which sets stuff in dirtyAttributes */
      attributes["id"] = query.lastInsertId().toInt();

      afterCreate();
    }
    afterSave();
    wasNewRecord = false;
  }
  else {
    qDebug() << "Query failed:" << query.lastError();
  }
  return result;
}

void Record::beforeCreate()
{
  set("created_at", QDateTime::currentDateTime());
}

void Record::beforeSave()
{
  set("updated_at", QDateTime::currentDateTime());
  //qDebug() << "Saving record:" << dirtyAttributes;
}

void Record::afterSave()
{
  QList<QString> dirtyKeys = dirtyAttributes.keys();
  for (int i = 0; i < dirtyKeys.count(); i++) {
    attributes[dirtyKeys[i]] = dirtyAttributes[dirtyKeys[i]];
  }
  dirtyAttributes.clear();
  //qDebug() << "New attributes:" << attributes;
  emit saved();
}

void Record::afterCreate()
{
}

bool Record::destroy(const QString &tableName)
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
  bool result = query.exec();
  if (result) {
    emit destroyed();
  }
  return result;
}

void Record::reset()
{
  dirtyAttributes.clear();
}

bool Record::operator==(const Record &other)
{
  return !isNew() && !other.isNew() && id() == other.id();
}

bool Record::executeFindQuery(QSqlQuery &query, const QString &tableName, const QString &select, const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate)
{
  QStringList queryStrings;
  queryStrings << "SELECT " << select << " FROM";
  queryStrings << tableName;
  if (!conditions.isEmpty()) {
    queryStrings << conditions;
  }
  if (!predicate.isEmpty()) {
    queryStrings << predicate;
  }
  QString queryString = queryStrings.join(" ");

  query.prepare(queryString);
  for (int i = 0; i < bindValues.size(); i++) {
    query.addBindValue(bindValues[i]);
  }
  return query.exec();
}
