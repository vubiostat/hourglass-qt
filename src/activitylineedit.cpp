#include "activitylineedit.h"

ActivityLineEdit::ActivityLineEdit(QWidget *parent)
  : QLineEdit(parent)
{
}

ActivityLineEdit::ActivityLineEdit(const QString &contents, QWidget *parent)
  : QLineEdit(contents, parent)
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
