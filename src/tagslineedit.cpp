#include "tagslineedit.h"

TagsLineEdit::TagsLineEdit(QWidget *parent)
  : AbstractLineEdit(parent)
{
}

TagsLineEdit::TagsLineEdit(const QString &contents, QWidget *parent)
  : AbstractLineEdit(contents, parent)
{
}

void TagsLineEdit::insertCompletion(const QString &completion)
{
  if (completer()->widget() != this)
    return;

  setSelection(startIndexOfCurrentWord(), cursorPosition());
  backspace();
  insert(completion);
}

QString TagsLineEdit::currentPrefix() const
{
  QString str = text();
  int position = cursorPosition();
  int startIndex = startIndexOfCurrentWord();
  return str.mid(startIndex, position - startIndex);
}
