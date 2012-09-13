#include "mainwindow.h"
#include "activitydialog.h"
#include "activity.h"
#include <QPalette>
#include <QDate>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
{
  m_ui.setupUi(this);

  QPalette pal(m_ui.saContentsWeek->palette());
  pal.setColor(QPalette::Background, Qt::white);
  m_ui.saContentsWeek->setPalette(pal);

  /* Set up activity tables */
  QDate today = QDate::currentDate();
  QDate sunday;
  if (today.dayOfWeek() == Qt::Sunday) {
    sunday = today;
  }
  else {
    sunday = today.addDays(- today.dayOfWeek());
  }

  m_ui.tblToday->setDate(today);
  connect(this, SIGNAL(activityCreated(const Activity &)),
      m_ui.tblToday->model(), SLOT(activityCreated(const Activity &)));

  if (today.dayOfWeek() == Qt::Sunday) {
    m_ui.tblSunday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblSunday->setDate(sunday);
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblSunday->model(), SLOT(activityCreated(const Activity &)));
  }

  if (today.dayOfWeek() == Qt::Monday) {
    m_ui.tblMonday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblMonday->setDate(sunday.addDays(1));
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblMonday->model(), SLOT(activityCreated(const Activity &)));
  }

  if (today.dayOfWeek() == Qt::Tuesday) {
    m_ui.tblTuesday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblTuesday->setDate(sunday.addDays(2));
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblTuesday->model(), SLOT(activityCreated(const Activity &)));
  }

  if (today.dayOfWeek() == Qt::Wednesday) {
    m_ui.tblWednesday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblWednesday->setDate(sunday.addDays(3));
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblWednesday->model(), SLOT(activityCreated(const Activity &)));
  }

  if (today.dayOfWeek() == Qt::Thursday) {
    m_ui.tblThursday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblThursday->setDate(sunday.addDays(4));
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblThursday->model(), SLOT(activityCreated(const Activity &)));
  }

  if (today.dayOfWeek() == Qt::Friday) {
    m_ui.tblFriday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblFriday->setDate(sunday.addDays(5));
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblFriday->model(), SLOT(activityCreated(const Activity &)));
  }

  if (today.dayOfWeek() == Qt::Saturday) {
    m_ui.tblSaturday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblSaturday->setDate(sunday.addDays(6));
    connect(this, SIGNAL(activityCreated(const Activity &)),
        m_ui.tblSaturday->model(), SLOT(activityCreated(const Activity &)));
  }
}

void MainWindow::on_btnStartActivity_clicked()
{
  startActivity();
}

void MainWindow::on_leActivity_returnPressed()
{
  startActivity();
}

void MainWindow::on_leTags_returnPressed()
{
  startActivity();
}

void MainWindow::on_btnAddEarlierActivity_clicked()
{
  ActivityDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::startActivity()
{
  Activity activity;
  activity.setNameWithProject(m_ui.leActivity->text());
  activity.setTagNames(m_ui.leTags->text());
  activity.setStartedAt(QDateTime::currentDateTime());
  activity.setRunning(true);

  if (activity.isValid()) {
    m_ui.leActivity->clear();
    m_ui.leTags->clear();
    activity.save();
    emit activityCreated(activity);
  }
}
