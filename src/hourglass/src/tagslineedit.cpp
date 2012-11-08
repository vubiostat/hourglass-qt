#include "tagslineedit.h"
#include <QAbstractItemView>
#include <QtDebug>

TagsLineEdit::TagsLineEdit(QWidget *parent)
  : AbstractLineEdit(parent)
{
}

TagsLineEdit::TagsLineEdit(const QString &contents, QWidget *parent)
  : AbstractLineEdit(contents, parent)
{
}

QRect TagsLineEdit::currentWordBoundaries() const
{
  QRect rect;
  int position = cursorPosition();
  QRegExp re(",\\s*");
  if (text().lastIndexOf(re, position) >= 0) {
    rect.setLeft(re.pos() + re.matchedLength());
  }
  else {
    rect.setLeft(0);
  }

  int right = text().indexOf(",", position);
  if (right >= 0) {
    rect.setRight(right - 1);
  }
  else {
    rect.setWidth(text().size());
  }
  return rect;
}
