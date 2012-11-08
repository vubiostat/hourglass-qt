#include "projecttotaldelegate.h"
#include <QtDebug>

ProjectTotalDelegate::ProjectTotalDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

void ProjectTotalDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItem newOption(option);

  /* Remove focus border */
  if (newOption.state & QStyle::State_HasFocus) {
    newOption.state ^= QStyle::State_HasFocus;
  }

  QStyledItemDelegate::paint(painter, newOption, index);
}

QSize ProjectTotalDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  size.setHeight(option.fontMetrics.height());
  return size;
}
