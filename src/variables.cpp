#include "variables.h"

// VariableMap
VariableMap::VariableMap(QObject *parent)
  : QObject(parent)
{
}

VariableMapList &VariableMap::addMapList(const QString &name)
{
  VariableMapList *mapList = new VariableMapList(name, this);
  connect(this, SIGNAL(destroyed()), mapList, SLOT(deleteLater()));
  mapLists << mapList;
  return *mapList;
}

void VariableMap::addVariable(const QString &name, const QString &value)
{
  variables.insert(name, QVariant(value));
}

void VariableMap::addVariable(const QString &name, int value)
{
  variables.insert(name, QVariant(value));
}

void VariableMap::addActivities(const QList<Activity> &activities)
{
  VariableMapList &activitiesList = addMapList("activities");
  for (int i = 0; i < activities.size(); i++) {
    VariableMap &activityMap = activitiesList.addMap();

    Activity activity = activities[i];
    activityMap.addVariable("id", activity.id);
    activityMap.addVariable("name", activity.name);
    activityMap.addVariable("startedAtISO8601", activity.startedAtISO8601());
    activityMap.addVariable("startedAtHM", activity.startedAtHM());
    activityMap.addVariable("endedAtHM", activity.endedAtHM());
    activityMap.addVariable("durationInWords", activity.durationInWords());
    activityMap.addVariable("isRunning", activity.isRunning() ? 0 : 1);
    activityMap.addVariable("projectName", activity.projectName);
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

  VariableMapList *mapList;
  for (int j = 0; j < mapLists.size(); j++) {
    mapList = mapLists.at(j);
    Teng::FragmentList_t &fragmentList = fragment.addFragmentList(mapList->name.toStdString());
    mapList->addToFragmentList(fragmentList);
  }
}

// VariableMapList
VariableMapList::VariableMapList(QString name, QObject *parent)
  : QObject(parent), name(name)
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
