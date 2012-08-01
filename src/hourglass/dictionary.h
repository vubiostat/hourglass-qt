#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QObject>
#include <ctemplate/template_dictionary.h>
#include "activity.h"

class Dictionary : public QObject
{
  Q_OBJECT

  public:
    Dictionary(QString name, QObject *parent = 0);
    Dictionary(ctemplate::TemplateDictionary *dictionary, QObject *parent = 0);
    ~Dictionary();

    Dictionary *addSectionDictionary(QString name);
    Dictionary *addIncludeDictionary(QString name, QString templateName);

    void setValue(QString name, QString value);
    void setValue(QString name, int value);
    void showSection(QString name);

    void addActivity(const Activity &activity);
    void addActivitySection(const Activity &activity);
    void addActivitySection(const Activity &activity, const QString &sectionName);
    void addActivitySection(const QList<Activity> &activities);
    void addActivitySection(const QList<Activity> &activities, const QString &sectionName);

    ctemplate::TemplateDictionary *object();

  private:
    bool deleteDictionary;
    ctemplate::TemplateDictionary *dictionary;
};

#endif
