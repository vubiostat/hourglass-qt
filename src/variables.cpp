#include "variables.h"

// VariableMap
VariableMap::VariableMap(QObject *parent)
  : QObject(parent)
{
}

VariableMap &VariableMap::addMap(QString name)
{
  VariableMap *map = new VariableMap(this);
  connect(this, SIGNAL(destroyed()), map, SLOT(deleteLater()));
  maps.insert(name, map);
  return *map;
}

VariableMapList &VariableMap::addMapList(QString name)
{
  VariableMapList *mapList = new VariableMapList(this);
  connect(this, SIGNAL(destroyed()), mapList, SLOT(deleteLater()));
  mapLists.insert(name, mapList);
  return *mapList;
}

void VariableMap::addVariable(QString name, QString value)
{
  variables.insert(name, QVariant(value));
}

void VariableMap::addVariable(QString name, int value)
{
  variables.insert(name, QVariant(value));
}

void VariableMap::addActivities(const QList<Activity> &activities)
{
  VariableMapList &activitiesList = addMapList("activities");
  for (int i = 0; i < activities.size(); i++) {
    VariableMap &activityMap = activitiesList.addMap();

    Activity activity = activities[i];
    activityMap.addVariable("id", activity.id());
    activityMap.addVariable("name", activity.name());
    activityMap.addVariable("startedAtISO8601", activity.startedAtISO8601());
    activityMap.addVariable("startedAtHM", activity.startedAtHM());
    activityMap.addVariable("endedAtHM", activity.endedAtHM());
    activityMap.addVariable("durationInWords", activity.durationInWords());
    activityMap.addVariable("isRunning", activity.isRunning() ? 0 : 1);
    activityMap.addVariable("projectName", activity.projectName());
  }
  addVariable("activityCount", activities.size());
}

/* The caller is responsible for deleting this */
Teng::Fragment_t *VariableMap::fragment()
{
  Teng::Fragment_t *fragment = new Teng::Fragment_t();
  addToFragment(*fragment);
  return fragment;
}

void VariableMap::addToFragment(Teng::Fragment_t &fragment)
{
  QMapIterator<QString, QVariant> i(variables);
  while (i.hasNext()) {
    i.next();
    switch (i.value().type()) {
      case QVariant::String:
        fragment.addVariable(i.key().toStdString(), i.value().toString().toStdString());
        break;
      case QVariant::Int:
        fragment.addVariable(i.key().toStdString(), (Teng::IntType_t) i.value().toInt());
        break;
    }
  }

  QMapIterator<QString, VariableMapList *> j(mapLists);
  while (j.hasNext()) {
    j.next();
    Teng::FragmentList_t &fragmentList = fragment.addFragmentList(j.key().toStdString());
    j.value()->addToFragmentList(fragmentList);
  }

  QMapIterator<QString, VariableMap *> k(maps);
  while (k.hasNext()) {
    k.next();
    Teng::Fragment_t &fragment2 = fragment.addFragment(k.key().toStdString());
    k.value()->addToFragment(fragment2);
  }
}

// VariableMapList
VariableMapList::VariableMapList(QObject *parent)
  : QObject(parent)
{
}

VariableMap &VariableMapList::addMap()
{
  VariableMap *map = new VariableMap(this);
  connect(this, SIGNAL(destroyed()), map, SLOT(deleteLater()));
  maps << map;
  return *map;
}

void VariableMapList::addToFragmentList(Teng::FragmentList_t &fragmentList)
{
  VariableMap *map;
  for (int i = 0; i < maps.size(); i++) {
    map = maps.at(i);
    Teng::Fragment_t &fragment = fragmentList.addFragment();
    map->addToFragment(fragment);
  }
}
