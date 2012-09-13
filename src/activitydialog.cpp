#include "activitydialog.h"

ActivityDialog::ActivityDialog(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  m_ui.setupUi(this);

  QDateTime now = QDateTime::currentDateTime();
  m_ui.deStartedAt->setDate(now.date());
  m_ui.teStartedAt->setTime(now.time());
  m_ui.teEndedAt->setTime(now.time());
  m_ui.deEndedAt->setDate(now.date());
  m_ui.deUntimedDate->setDate(now.date());
}
