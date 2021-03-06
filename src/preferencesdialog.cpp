#include "preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  m_ui.setupUi(this);

  m_ui.cbNoConcurrentActivities->setChecked(m_settings.value("noConcurrentActivities", true).toBool());
  m_ui.cbStopActivitiesOnExit->setChecked(m_settings.value("stopActivitiesOnExit", true).toBool());
  m_ui.cbShowTrayIcon->setChecked(m_settings.value("showTrayIcon", true).toBool());
}

bool PreferencesDialog::showTrayIcon() const
{
  return m_ui.cbShowTrayIcon->isChecked();
}

void PreferencesDialog::accept()
{
  m_settings.setValue("noConcurrentActivities", m_ui.cbNoConcurrentActivities->isChecked());
  m_settings.setValue("stopActivitiesOnExit", m_ui.cbStopActivitiesOnExit->isChecked());
  m_settings.setValue("showTrayIcon", m_ui.cbShowTrayIcon->isChecked());
  QDialog::accept();
}
