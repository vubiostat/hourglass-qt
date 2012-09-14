#include "activitydelegate.h"
#include <QtDebug>

ActivityDelegate::ActivityDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

void ActivityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItem newOption(option);
  switch (index.column()) {
    case 3:
    case 4:
      newOption.rect.adjust(15, 0, 15, 0);
      break;
  }
  /* Remove focus border */
  if (newOption.state & QStyle::State_HasFocus) {
    newOption.state ^= QStyle::State_HasFocus;
  }

  QStyledItemDelegate::paint(painter, newOption, index);
}

QSize ActivityDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  switch (index.column()) {
    case 0:
    case 2:
      size.setWidth(option.fontMetrics.width("00:00") + 7);
      break;
    case 1:
      size.setWidth(option.fontMetrics.width("-"));
      break;
    case 3:
    case 4:
      size.setWidth(size.width() + 15);
      break;
  }
  return size;
}
