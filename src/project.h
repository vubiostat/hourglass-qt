#ifndef PROJECT_H
#define PROJECT_H

#include "model.h"

class Project : public Model
{
  Q_OBJECT

  public:
    static QList<Project> find(QString conditions);
    static Project findById(int id);
    static QList<QString> distinctNames();

    Project(QObject *parent = 0);
    Project(QMap<QString, QVariant> &attributes, QObject *parent = 0);

    int id();
    QString name();

  private:
    static const QString findQuery;
    static const QString distinctNamesQuery;
};

#endif
