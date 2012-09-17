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

QList<Project *> Project::find(const QString &conditions, QObject *parent)
{
  return Record::find<Project>("projects", conditions, parent);
}

QList<Project *> Project::find(const QString &conditions, const QList<QVariant> &bindValues, QObject *parent)
{
  return Record::find<Project>("projects", conditions, bindValues, parent);
}

QList<Project *> Project::findById(int id, QObject *parent)
{
  return Record::findById<Project>("projects", id, parent);
}

int Project::findOrCreateByName(const QString &name)
{
  int id = -1;
  QList<QVariant> bindValues;
  bindValues << QVariant(name);
  QList<Project *> projects = find("WHERE name = ?", bindValues);

  if (projects.size() > 0) {
    id = projects[0]->id();
    while (projects.isEmpty()) {
      projects.takeLast()->deleteLater();
    }
  }
  else {
    QSqlDatabase database = Project::database();
    QSqlQuery query(database);
    query.prepare(insertQuery);
    query.addBindValue(QVariant(name));
    if (query.exec()) {
      id = query.lastInsertId().toInt();
    }
  }
  return id;
}

QList<QString> Project::distinctNames()
{
  QSqlDatabase database = Project::database();
  QSqlQuery query = database.exec(distinctNamesQuery);

  QList<QString> names;
  while (query.next()) {
    names << query.value(0).toString();
  }
  return names;
}

bool Project::save()
{
  return Record::save("projects");
}

QString Project::name() const
{
  return get("name").toString();
}
