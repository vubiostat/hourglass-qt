#ifndef PROJECT_H
#define PROJECT_H

#include "model.h"

class Project : public Model
{
  Q_OBJECT

  public:
    static QList<Project> find(QString conditions);
    static QList<Project> find(QString conditions, const QList<QVariant> &bindValues);
    static Project findById(int id);
    static int findOrCreateByName(const QString &name);
    static QList<QString> distinctNames();

    Project(QObject *parent = 0);
    Project(QMap<QString, QVariant> &attributes, QObject *parent = 0);

    int id();
    QString name();

  private:
    static const QString findQuery;
    static const QString insertQuery;
    static const QString distinctNamesQuery;
};

#endif
