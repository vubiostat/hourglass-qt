#ifndef VARIABLES_H
#define VARIABLES_H

#include <QObject>
#include <QVariant>
#include <QMap>
#include "tengstructs.h"
#include "activity.h"

class VariableMap;
class VariableMapList;

class VariableMap : public QObject
{
  Q_OBJECT

  public:
    VariableMap(QObject *parent = 0);

    VariableMapList &addMapList(const QString &name);
    void addVariable(const QString &name, const QString &value);
    void addVariable(const QString &name, int value);
    void addActivities(const QList<Activity> &activities);
    Teng::Fragment_t *fragment();

  private:
    QMap<QString, QVariant> variables;
    QList<VariableMapList *> mapLists;

    void addToFragment(Teng::Fragment_t &fragment);
    friend class VariableMapList;
};

class VariableMapList : public QObject
{
  Q_OBJECT

  public:
    VariableMapList(QString name, QObject *parent = 0);
    VariableMap &addMap();

  private:
    QString name;
    QList<VariableMap *> maps;

    void addToFragmentList(Teng::FragmentList_t &fragmentList);
    friend class VariableMap;
};

#endif
