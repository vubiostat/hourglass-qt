#ifndef __ACTIVITYDIALOG_H
#define __ACTIVITYDIALOG_H

#include <QDialog>
#include "ui_activitydialog.h"

class ActivityDialog : public QDialog
{
  Q_OBJECT

  public:
    ActivityDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

  private:
    Ui::ActivityDialog m_ui;
};

#endif
