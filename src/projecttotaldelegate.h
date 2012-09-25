#ifndef __PROJECTTOTALDELEGATE_H
#define __PROJECTTOTALDELEGATE_H

#include <QStyledItemDelegate>

class ProjectTotalDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    ProjectTotalDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
