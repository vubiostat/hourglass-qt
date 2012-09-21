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

QList<Tag *> Tag::find(QObject *parent)
{
  return Record::find<Tag>("tags", parent);
}

QList<Tag *> Tag::find(const QString &conditions, QObject *parent)
{
  return Record::find<Tag>("tags", conditions, parent);
}

QList<Tag *> Tag::find(const QString &conditions, const QList<QVariant> &bindValues, QObject *parent)
{
  return Record::find<Tag>("tags", conditions, bindValues, parent);
}

Tag *Tag::findById(int id, QObject *parent)
{
  return Record::findById<Tag>("tags", id, parent);
}

Tag *Tag::findOrCreateByName(const QString &name, QObject *parent)
{
  Tag *tag = NULL;
  QList<QVariant> bindValues;
  bindValues << QVariant(name);
  QList<Tag *> tags = find("WHERE name = ?", bindValues, parent);

  for (int i = 0; i < tags.count(); i++) {
    if (i == 0) {
      tag = tags[i];
    }
    else {
      tags[i]->deleteLater();
    }
  }

  if (tag == NULL) {
    tag = new Tag(parent);
    tag->setName(name);

    /* FIXME: handle potential save failure */
    tag->save();
  }
  return tag;
}

QList<Tag *> Tag::findActivityTags(int activityId, QObject *parent)
{
  QSqlDatabase database = Tag::database();
  QSqlQuery query(database);
  query.prepare(findActivityTagsQuery);
  query.bindValue(0, activityId);
  query.exec();

  QList<Tag *> result;
  while (query.next()) {
    QMap<QString, QVariant> attributes;
    attributes["id"] = query.value(0);
    attributes["name"] = query.value(1);
    result << new Tag(attributes, false, parent);
  }
  return result;
}

QStringList Tag::distinctNames()
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
