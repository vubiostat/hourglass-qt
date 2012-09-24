#include "projecttotaldelegate.h"
#include <QtDebug>

ProjectTotalDelegate::ProjectTotalDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

QSize ProjectTotalDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  size.setHeight(option.fontMetrics.height());
  return size;
}
