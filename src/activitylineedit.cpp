#include "activitylineedit.h"

ActivityLineEdit::ActivityLineEdit(QWidget *parent)
  : AbstractLineEdit(parent)
{
}

ActivityLineEdit::ActivityLineEdit(const QString &contents, QWidget *parent)
  : AbstractLineEdit(contents, parent)
{
}

QString ActivityLineEdit::activityName() const
{
  return text().section('@', 0, 0);
}

QString ActivityLineEdit::projectName() const
{
  return text().section('@', 1, 1);
}

void ActivityLineEdit::insertCompletion(const QString &completion)
{
  if (completer()->widget() != this)
    return;

  setSelection(0, cursorPosition());
  backspace();
  insert(completion);
}

QString ActivityLineEdit::currentPrefix() const
{
  return text();
}
