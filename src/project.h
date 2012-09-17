#ifndef __PROJECT_H
#define __PROJECT_H

#include "record.h"

class Project : public Record
{
  Q_OBJECT

  public:
    static QList<Project *> find(const QString &conditions, QObject *parent = 0);
    static QList<Project *> find(const QString &conditions, const QList<QVariant> &bindValues, QObject *parent = 0);
    static QList<Project *> findById(int id, QObject *parent);
    static int findOrCreateByName(const QString &name);
    static QList<QString> distinctNames();

    Project(QObject *parent = 0) : Record(parent) {}
    Project(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0)
      : Record(attributes, newRecord, parent) {}

    bool save();

    QString name() const;

  private:
    static const QString findQuery;
    static const QString insertQuery;
    static const QString distinctNamesQuery;
};

#endif
