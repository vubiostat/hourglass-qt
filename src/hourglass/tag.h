#ifndef TAG_H
#define TAG_H

#include "model.h"

class Tag : public Model
{
  Q_OBJECT

  public:
    static QList<Tag> find(QString conditions);
    static QList<Tag> findActivityTags(int activityId);
    static QList<QString> distinctNames();

    Tag(QObject *parent = 0);
    Tag(QMap<QString, QVariant> &attributes, QObject *parent = 0);

    bool save();

    QString name();

  private:
    static const QString findQuery;
    static const QString findActivityTagsQuery;
    static const QString distinctNamesQuery;
};

#endif
