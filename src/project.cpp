#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "project.h"

const QString Project::findQuery = QString(
    "SELECT id, name FROM projects");

const QString Project::insertQuery = QString(
    "INSERT INTO projects (name) VALUES(?)");

const QString Project::distinctNamesQuery = QString(
    "SELECT DISTINCT name FROM projects ORDER BY name");

QList<Project> Project::find(QString conditions)
{
  QList<QVariant> emptyBindValues;
  return find(conditions, emptyBindValues);
}

QList<Project> Project::find(QString conditions, const QList<QVariant> &bindValues)
{
  QStringList queryStrings;
  queryStrings << findQuery;
  if (!conditions.isEmpty()) {
    queryStrings << conditions;
  }
  qDebug() << queryStrings.join(" ");

  QSqlDatabase &database = getDatabase();
  QSqlQuery query(database);
  query.prepare(queryStrings.join(" "));
  for (int i = 0; i < bindValues.size(); i++) {
    query.addBindValue(bindValues[i]);
  }
  query.exec();

  QList<Project> result;
  while (query.next()) {
    QMap<QString, QVariant> attributes;
    attributes["id"] = query.value(0);
    attributes["name"] = query.value(1);

    Project project(attributes);
    result << project;
  }
  return result;
}

Project Project::findById(int id)
{
  QList<Project> projects = find(QString("WHERE id = %1").arg(id));
  if (projects.size() != 1) {
    return Project();
  }
  return projects[0];
}

int Project::findOrCreateByName(const QString &name)
{
  QList<QVariant> bindValues;
  bindValues << QVariant(name);
  QList<Project> projects = find("WHERE name = ?", bindValues);
  if (projects.size() > 0) {
    return projects[0].id();
  }
  else {
    QSqlDatabase &database = getDatabase();
    QSqlQuery query(database);
    query.prepare(insertQuery);
    query.addBindValue(QVariant(name));
    if (query.exec()) {
      return query.lastInsertId().toInt();
    }
    else {
      return -1;
    }
  }
}

QList<QString> Project::distinctNames()
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query = database.exec(distinctNamesQuery);

  QList<QString> names;
  while (query.next()) {
    names << query.value(0).toString();
  }
  return names;
}

Project::Project(QObject *parent)
  : Model(parent)
{
}

Project::Project(QMap<QString, QVariant> &attributes, QObject *parent)
  : Model(attributes, parent)
{
}

int Project::id()
{
  return get("id").toInt();
}

QString Project::name()
{
  return get("name").toString();
}
