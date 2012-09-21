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

QRect ActivityLineEdit::currentWordBoundaries() const
{
  return QRect(0, 0, text().size(), 0);
}
