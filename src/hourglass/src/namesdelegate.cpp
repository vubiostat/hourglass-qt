#include "namesdelegate.h"

NamesDelegate::NamesDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

void NamesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  /* Add padding */
  QStyleOptionViewItem newOption(option);
  //newOption.rect.adjust(0, 5, 0, 5);

  QStyledItemDelegate::paint(painter, newOption, index);
}

QSize NamesDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  //size.setHeight(size.height() + 10);
  return size;
}
