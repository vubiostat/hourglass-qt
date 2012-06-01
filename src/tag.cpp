#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "tag.h"

const QString Tag::findQuery = QString(
    "SELECT id, name FROM tags");

const QString Tag::findActivityTagsQuery = QString(
    "SELECT tags.id, tags.name FROM activity_tags "
    "JOIN tags ON activity_tags.tag_id = tags.id "
    "WHERE activity_tags.activity_id = ?");

const QString Tag::distinctNamesQuery = QString(
    "SELECT DISTINCT name FROM tags ORDER BY name");

QList<Tag> Tag::find(QString conditions)
{
  QStringList queryStrings;
  queryStrings << findQuery;
  if (!conditions.isEmpty()) {
    queryStrings << conditions;
  }
  //qDebug() << queryStrings.join(" ");

  QSqlDatabase &database = getDatabase();
  QSqlQuery query = database.exec(queryStrings.join(" "));

  QList<Tag> result;
  while (query.next()) {
    QMap<QString, QVariant> attributes;
    attributes["id"] = query.value(0);
    attributes["name"] = query.value(1);

    Tag tag(attributes);
    result << tag;
  }
  return result;
}

QList<Tag> Tag::findActivityTags(int activityId)
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query(database);
  query.prepare(findActivityTagsQuery);
  query.bindValue(0, activityId);
  query.exec();

  QList<Tag> result;
  while (query.next()) {
    QMap<QString, QVariant> attributes;
    attributes["id"] = query.value(0);
    attributes["name"] = query.value(1);

    Tag tag(attributes);
    result << tag;
  }
  return result;
}

QList<QString> Tag::distinctNames()
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query = database.exec(distinctNamesQuery);

  QList<QString> names;
  while (query.next()) {
    names << query.value(0).toString();
  }
  return names;
}

Tag::Tag(QObject *parent)
  : Model(parent)
{
}

Tag::Tag(QMap<QString, QVariant> &attributes, QObject *parent)
  : Model(attributes, parent)
{
}

int Tag::id()
{
  return get("id").toInt();
}

QString Tag::name()
{
  return get("name").toString();
}
