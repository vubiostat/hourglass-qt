#ifndef __TAG_H
#define __TAG_H

#include "record.h"

class Tag : public Record
{
  Q_OBJECT

  public:
    static QList<Tag *> find(QObject *parent = 0);
    static QList<Tag *> find(const QString &conditions, QObject *parent = 0);
    static QList<Tag *> find(const QString &conditions, const QList<QVariant> &bindValues, QObject *parent = 0);
    static QList<Tag *> findById(int id, QObject *parent = 0);
    static Tag *findOrCreateByName(const QString &name, QObject *parent = 0);
    static QList<Tag *> findActivityTags(int activityId, QObject *parent = 0);
    static QList<QString> distinctNames();

    Tag(QObject *parent = 0) : Record(parent) {}
    Tag(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0)
      : Record(attributes, newRecord, parent) {}

    QString name() const;
    void setName(const QString &name);

    bool save();

  private:
    static const QString findActivityTagsQuery;
    static const QString distinctNamesQuery;
};

#endif
