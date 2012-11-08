#ifndef __ACTIVITYDELEGATE_H
#define __ACTIVITYDELEGATE_H

#include <QStyledItemDelegate>

class ActivityDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    ActivityDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
