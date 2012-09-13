#ifndef __TAG_H
#define __TAG_H

#include "record.h"

class Tag : public Record
{
  Q_OBJECT

  public:
    static QList<Tag> find();
    static QList<Tag> find(QString conditions);
    static QList<Tag> find(QString conditions, const QList<QVariant> &bindValues);
    static Tag findById(int id);
    static Tag findOrCreateByName(const QString &name);
    static QList<Tag> findActivityTags(int activityId);
    static QList<QString> distinctNames();

    Tag(QObject *parent = 0) : Record(parent) {}
    Tag(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0)
      : Record(attributes, newRecord, parent) {}

    QString name() const;
    void setName(const QString &name);

    bool save();

  private:
    static const QString findActivityTagsQuery;
    static const QString distinctNamesQuery;
};

#endif
