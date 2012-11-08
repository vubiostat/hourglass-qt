#include "currentactivitydelegate.h"
#include "currentactivitytableview.h"
#include <QApplication>
#include <QtDebug>

CurrentActivityDelegate::CurrentActivityDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

void CurrentActivityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (index.column() == 4) {
    /* Draw a stop button */
    CurrentActivityTableView *view = qobject_cast<CurrentActivityTableView *>(QObject::parent());
    if (view != NULL) {
      QStyleOptionButton buttonOption;
      buttonOption.rect = option.rect;
      buttonOption.text = "Stop activity";

      QSize buttonSize = view->stopButtonSize();
      if (buttonSize.isValid()) {
        /* Center the button vertically */
        int diff = buttonOption.rect.height() - buttonSize.height();
        buttonOption.rect.setHeight(buttonSize.height());
        buttonOption.rect.adjust(0, diff / 2, 0, diff / 2);
      }

      if (index.data().toInt() >= 0) {
        buttonOption.state = QStyle::State_Enabled;

        /* Find out if the button is being clicked */
        /*
        if (QApplication::mouseButtons() & Qt::LeftButton) {
          QPoint globalCursorPos = QCursor::pos();
          QPoint widgetPos = view->mapFromGlobal(globalCursorPos);
          if (buttonOption.rect.contains(widgetPos)) {
            buttonOption.state |= QStyle::State_Sunken;
          }
        }
        */
      }
      else {
        /* This is a placeholder button */
        buttonOption.state = QStyle::State_None;
      }

      view->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
    }
    else {
      qDebug() << "Couldn't draw stop button!";
    }
  }
  else {
    QStyleOptionViewItem newOption(option);

    /* Remove focus border */
    if (newOption.state & QStyle::State_HasFocus) {
      newOption.state ^= QStyle::State_HasFocus;
    }

    switch (index.column()) {
      case 1:
      case 2:
        /* Add left padding to project name and tag names */
        newOption.rect.adjust(15, 0, 15, 0);
        break;
    }
    QStyledItemDelegate::paint(painter, newOption, index);
  }
}

QSize CurrentActivityDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  switch (index.column()) {
    case 1:
    case 2:
      size.setWidth(size.width() + 15);
      break;
  }
  return size;
}
