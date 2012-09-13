#ifndef __PROJECT_H
#define __PROJECT_H

#include "record.h"

class Project : public Record
{
  Q_OBJECT

  public:
    static QList<Project> find(QString conditions);
    static QList<Project> find(QString conditions, const QList<QVariant> &bindValues);
    static Project findById(int id);
    static int findOrCreateByName(const QString &name);
    static QList<QString> distinctNames();

    Project(QObject *parent = 0) : Record(parent) {}
    Project(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0)
      : Record(attributes, newRecord, parent) {}

    bool save();

    QString name() const;

  private:
    static const QString findQuery;
    static const QString insertQuery;
    static const QString distinctNamesQuery;
};

#endif
