#ifndef __CURRENTACTIVITYDELEGATE_H
#define __CURRENTACTIVITYDELEGATE_H

#include <QStyledItemDelegate>

class CurrentActivityDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    CurrentActivityDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
