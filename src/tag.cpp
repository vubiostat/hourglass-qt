#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "tag.h"

const QString Tag::findActivityTagsQuery = QString(
    "SELECT tags.id, tags.name FROM activities_tags "
    "JOIN tags ON activities_tags.tag_id = tags.id "
    "WHERE activities_tags.activity_id = ?");

const QString Tag::distinctNamesQuery = QString(
    "SELECT DISTINCT name FROM tags ORDER BY name");

QList<Tag> Tag::find()
{
  return Record::find<Tag>("tags");
}

QList<Tag> Tag::find(QString conditions)
{
  return Record::find<Tag>("tags", conditions);
}

QList<Tag> Tag::find(QString conditions, const QList<QVariant> &bindValues)
{
  return Record::find<Tag>("tags", conditions, bindValues);
}

Tag Tag::findById(int id)
{
  return Record::findById<Tag>("tags", id);
}

Tag Tag::findOrCreateByName(const QString &name)
{
  QList<QVariant> bindValues;
  bindValues << QVariant(name);
  QList<Tag> tags = find("WHERE name = ?", bindValues);
  if (tags.size() > 0) {
    return tags[0];
  }
  else {
    Tag tag;
    tag.setName(name);
    tag.save();
    return tag;
  }
}

QList<Tag> Tag::findActivityTags(int activityId)
{
  QSqlDatabase database = Tag::database();
  QSqlQuery query(database);
  query.prepare(findActivityTagsQuery);
  query.bindValue(0, activityId);
  query.exec();

  QList<Tag> result;
  while (query.next()) {
    QMap<QString, QVariant> attributes;
    attributes["id"] = query.value(0);
    attributes["name"] = query.value(1);

    Tag tag(attributes, false);
    result << tag;
  }
  return result;
}

QList<QString> Tag::distinctNames()
{
  QSqlDatabase database = Tag::database();
  QSqlQuery query = database.exec(distinctNamesQuery);

  QList<QString> names;
  while (query.next()) {
    names << query.value(0).toString();
  }
  return names;
}

QString Tag::name() const
{
  return get("name").toString();
}

void Tag::setName(const QString &name)
{
  set("name", name);
}

bool Tag::save()
{
  return Record::save("tags");
}
