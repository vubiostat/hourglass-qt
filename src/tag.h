#ifndef TAG_H
#define TAG_H

#include "model.h"
#include "activity.h"

class Tag : public Model
{
  Q_OBJECT

  public:
    static QList<Tag> find(QString conditions);
    static QList<Tag> findActivityTags(Activity &activity);
    static QList<QString> distinctNames();

    Tag(QObject *parent = 0);
    Tag(QMap<QString, QVariant> &attributes, QObject *parent = 0);

    int id();
    QString name();

  private:
    static const QString findQuery;
    static const QString findActivityTagsQuery;
    static const QString distinctNamesQuery;
};

#endif
