#include "mainwindow.h"
#include "activitydelegate.h"
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

  if (today.dayOfWeek() == Qt::Sunday) {
    m_ui.tblSunday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblSunday->setDate(sunday);
  }

  if (today.dayOfWeek() == Qt::Monday) {
    m_ui.tblMonday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblMonday->setDate(sunday.addDays(1));
  }

  if (today.dayOfWeek() == Qt::Tuesday) {
    m_ui.tblTuesday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblTuesday->setDate(sunday.addDays(2));
  }

  if (today.dayOfWeek() == Qt::Wednesday) {
    m_ui.tblWednesday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblWednesday->setDate(sunday.addDays(3));
  }

  if (today.dayOfWeek() == Qt::Thursday) {
    m_ui.tblThursday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblThursday->setDate(sunday.addDays(4));
  }

  if (today.dayOfWeek() == Qt::Friday) {
    m_ui.tblFriday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblFriday->setDate(sunday.addDays(5));
  }

  if (today.dayOfWeek() == Qt::Saturday) {
    m_ui.tblSaturday->setModel(m_ui.tblToday->model());
  }
  else {
    m_ui.tblSaturday->setDate(sunday.addDays(6));
  }
}
