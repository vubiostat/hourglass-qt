#ifndef __ACTIVITYLINEEDIT_H
#define __ACTIVITYLINEEDIT_H

#include "abstractlineedit.h"

class ActivityLineEdit : public AbstractLineEdit
{
  Q_OBJECT

  public:
    ActivityLineEdit(QWidget *parent = 0);
    ActivityLineEdit(const QString &contents, QWidget *parent = 0);

    QString activityName() const;
    QString projectName() const;

  protected:
    QRect currentWordBoundaries() const;
};

#endif
