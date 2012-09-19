#include "mainwindow.h"
#include "activitydialog.h"
#include "activity.h"
#include "currentactivitytablemodel.h"
#include "activitytablemodel.h"
#include "activitydelegate.h"
#include "currentactivitydelegate.h"
#include <QPalette>
#include <QDate>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
{
  m_ui.setupUi(this);

  m_recordManager = new RecordManager<Activity>;

  /* Set background of week tab to white */
  QPalette weekPal(m_ui.saContentsWeek->palette());
  weekPal.setColor(QPalette::Background, Qt::white);
  m_ui.saContentsWeek->setPalette(weekPal);

  /* Set background of current activity table to transparent */
  QPalette currentPal(m_ui.tblCurrent->palette());
  currentPal.setColor(QPalette::Base, Qt::transparent);
  m_ui.tblCurrent->setPalette(currentPal);

  /* Set up activity tables */
  m_ui.tblCurrent->setModel(new CurrentActivityTableModel(m_recordManager, this));
  m_ui.tblCurrent->setItemDelegate(new CurrentActivityDelegate(this));
  connect(this, SIGNAL(activityCreated(QSharedPointer<Activity>)),
      m_ui.tblCurrent->model(), SLOT(activityCreated(QSharedPointer<Activity>)));

  QDate today = QDate::currentDate();
  QDate sunday;
  if (today.dayOfWeek() == Qt::Sunday) {
    sunday = today;
  }
  else {
    sunday = today.addDays(- today.dayOfWeek());
  }

  /* Today */
  setupActivityTableView(m_ui.tblToday, today);

  /* Sunday */
  if (today.dayOfWeek() == Qt::Sunday) {
    setupActivityTableView(m_ui.tblSunday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblSunday, sunday);
  }

  /* Monday */
  if (today.dayOfWeek() == Qt::Monday) {
    setupActivityTableView(m_ui.tblMonday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblMonday, sunday.addDays(1));
  }

  /* Tuesday */
  if (today.dayOfWeek() == Qt::Tuesday) {
    setupActivityTableView(m_ui.tblTuesday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblTuesday, sunday.addDays(2));
  }

  /* Wednesday */
  if (today.dayOfWeek() == Qt::Wednesday) {
    setupActivityTableView(m_ui.tblWednesday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblWednesday, sunday.addDays(3));
  }

  /* Thursday */
  if (today.dayOfWeek() == Qt::Thursday) {
    setupActivityTableView(m_ui.tblThursday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblThursday, sunday.addDays(4));
  }

  /* Friday */
  if (today.dayOfWeek() == Qt::Friday) {
    setupActivityTableView(m_ui.tblFriday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblFriday, sunday.addDays(5));
  }

  /* Saturday */
  if (today.dayOfWeek() == Qt::Saturday) {
    setupActivityTableView(m_ui.tblSaturday, m_ui.tblToday->model());
  }
  else {
    setupActivityTableView(m_ui.tblSaturday, sunday.addDays(6));
  }
}

MainWindow::~MainWindow()
{
  delete m_recordManager;
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
  ActivityDialog *dialog = new ActivityDialog(this);
  dialog->setModal(true);
  if (dialog->exec() == QDialog::Accepted) {
    m_recordManager->addRecordPointer(dialog->activity());
    emit activityCreated(dialog->activity());
  }
  dialog->deleteLater();
}

void MainWindow::editActivity(QSharedPointer<Activity> activity)
{
  ActivityDialog *dialog = new ActivityDialog(activity, this);
  dialog->setModal(true);
  dialog->exec();
  dialog->deleteLater();
}

void MainWindow::startActivity()
{
  Activity *activity = new Activity();
  activity->setNameWithProject(m_ui.leActivity->text());
  activity->setTagNames(m_ui.leTags->text());
  activity->setStartedAt(QDateTime::currentDateTime());
  activity->setRunning(true);

  if (activity->isValid()) {
    m_ui.leActivity->clear();
    m_ui.leTags->clear();
    activity->save();
    QSharedPointer<Activity> ptr = m_recordManager->addRecord(activity);
    emit activityCreated(ptr);
  }
  else {
    activity->deleteLater();
  }
}

void MainWindow::setupActivityTableView(ActivityTableView *view, const QDate &date)
{
  ActivityTableModel *model = new ActivityTableModel(date, m_recordManager, this);
  connect(this, SIGNAL(activityCreated(QSharedPointer<Activity>)),
      model, SLOT(activityCreated(QSharedPointer<Activity>)));
  connect(model, SIGNAL(activityStarted()),
      m_ui.tblCurrent->model(), SLOT(refreshActivities()));
  setupActivityTableView(view, model);
}

void MainWindow::setupActivityTableView(ActivityTableView *view, ActivityTableModel *model)
{
  view->setModel(model);
  view->setItemDelegate(new ActivityDelegate(this));
  connect(view, SIGNAL(editActivity(QSharedPointer<Activity>)),
      SLOT(editActivity(QSharedPointer<Activity>)));
}
