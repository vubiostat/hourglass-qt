#ifndef __PREFERENCESDIALOG_H
#define __PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>
#include "ui_preferencesdialog.h"

class PreferencesDialog : public QDialog
{
  Q_OBJECT

  public:
    PreferencesDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    bool showTrayIcon() const;

  public slots:
    void accept();

  private:
    Ui::PreferencesDialog m_ui;
    QSettings m_settings;
};


#endif
