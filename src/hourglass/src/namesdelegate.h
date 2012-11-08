#ifndef __NAMESDELEGATE_H
#define __NAMESDELEGATE_H

#include <QStyledItemDelegate>

class NamesDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    NamesDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
