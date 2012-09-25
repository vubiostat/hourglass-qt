#include "mainwindow.h"
#include "activitydialog.h"
#include "activity.h"
#include "activitytablemodel.h"
#include "currentactivitytablemodel.h"
#include "projecttotalslistmodel.h"
#include "activitydelegate.h"
#include "currentactivitydelegate.h"
#include "namesdelegate.h"
#include "projecttotaldelegate.h"
#include "aboutdialog.h"
#include "preferencesdialog.h"
#include <QPalette>
#include <QDate>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags), m_activityCompleter(NULL),
    m_tagCompleter(NULL), m_activityCompleterModel(NULL),
    m_tagCompleterModel(NULL), m_trayIconAvailable(false),
    m_showTrayIcon(false), m_trayIconMenu(NULL), m_trayIcon(NULL)
{
  m_ui.setupUi(this);
  setWindowIcon(QIcon(":/icons/hourglass.png"));
  m_ui.actionAbout_Qt->setIcon(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"));

  m_trayIconAvailable = QSystemTrayIcon::isSystemTrayAvailable();
  if (m_trayIconAvailable) {
    m_showTrayIcon = m_settings.value("showTrayIcon", true).toBool();
    if (m_showTrayIcon) {
      createTrayIcon();
    }
  }
  else {
    qDebug() << "Couldn't display tray icon";
  }

  /* Set background of week tab to white */
  QPalette weekPal(m_ui.saContentsWeek->palette());
  weekPal.setColor(QPalette::Background, Qt::white);
  m_ui.saContentsWeek->setPalette(weekPal);

  /* Set background of current activity table to transparent */
  QPalette currentPal(m_ui.tblCurrent->palette());
  currentPal.setColor(QPalette::Base, Qt::transparent);
  m_ui.tblCurrent->setPalette(currentPal);

  /* Setup activity completer */
  m_activityCompleter = new QCompleter(this);
  m_activityCompleterModel = new ActivityNamesListModel(this);
  m_activityCompleter->setModel(m_activityCompleterModel);
  m_activityCompleter->setCompletionMode(QCompleter::PopupCompletion);
  m_activityCompleter->setCaseSensitivity(Qt::CaseInsensitive);

  QAbstractItemView *activityPopup = m_activityCompleter->popup();
  activityPopup->setItemDelegate(new NamesDelegate(activityPopup));
  activityPopup->setFrameShadow(QFrame::Plain);

  m_ui.leActivity->setCompleter(m_activityCompleter);

  /* Setup tag completer */
  m_tagCompleter = new QCompleter(this);
  m_tagCompleterModel = new TagNamesListModel(this);
  m_tagCompleter->setModel(m_tagCompleterModel);
  m_tagCompleter->setCompletionMode(QCompleter::PopupCompletion);
  m_tagCompleter->setCaseSensitivity(Qt::CaseInsensitive);

  QAbstractItemView *tagPopup = m_tagCompleter->popup();
  tagPopup->setItemDelegate(new NamesDelegate(tagPopup));
  tagPopup->setFrameShadow(QFrame::Plain);

  m_ui.leTags->setCompleter(m_tagCompleter);

  /* Set up activity tables */
  m_recordManager = new RecordManager<Activity>;

  m_ui.tblCurrent->setModel(new CurrentActivityTableModel(m_recordManager, this));
  m_ui.tblCurrent->setItemDelegate(new CurrentActivityDelegate(m_ui.tblCurrent));
  connect(this, SIGNAL(activityCreated(QSharedPointer<Activity>)),
      m_ui.tblCurrent->model(), SLOT(created(QSharedPointer<Activity>)));

  setupViews();

  /* Set a timer to go off hourly to check if we should switch days */
  m_dayTimer = new QTimer(this);
  m_dayTimer->setInterval(3600000);
  connect(m_dayTimer, SIGNAL(timeout()), this, SLOT(setupViews()));

  QTime nowTime = QTime::currentTime();
  int msecsToHour = 3600000 - (nowTime.minute() * 60000) - 
    (nowTime.second() * 1000) - nowTime.msec();
  QTimer::singleShot(msecsToHour, this, SLOT(setupViews()));
  QTimer::singleShot(msecsToHour, m_dayTimer, SLOT(start()));
}

MainWindow::~MainWindow()
{
  delete m_recordManager;
}

bool MainWindow::isTrayIconShown() const
{
  return m_showTrayIcon;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
  QMainWindow::paintEvent(event);

  QSize size = m_ui.btnStartActivity->size();
  if (m_startButtonSize != size) {
    m_startButtonSize = size;
    m_ui.tblCurrent->setStopButtonSize(size);
  }
}

void MainWindow::setupViews()
{
  qDebug() << "Setting up views...";

  QDate today = QDate::currentDate();
  QDate monday; // Monday is the first day of the week (according to Qt)
  if (today.dayOfWeek() == Qt::Monday) {
    monday = today;
  }
  else {
    monday = today.addDays(1 - today.dayOfWeek());
  }

  AbstractActivityModel *todayTableModel = m_ui.tblToday->model();
  AbstractActivityModel *todayListModel = m_ui.lvTodayTotals->model();

  /* If the views have already been setup, check to see if any need to
   * be changed (because the date has changed) */
  if (todayTableModel != NULL) {
    if (todayTableModel->date() == today) {
      /* Nothing to do */
      qDebug() << "Nothing to do.";
      return;
    }
    else {
      QDate previousDate = todayTableModel->date();
      if (previousDate.weekNumber() != today.weekNumber()) {
        /* We've switched weeks, need to reset everything */
        qDebug() << "Switched weeks, resetting models";
      }
      else {
        /* Just need to change today's view */
        qDebug() << "Switching today's models";
        AbstractActivityModel *tableModel = NULL, *listModel = NULL;
        switch (today.dayOfWeek()) {
          case Qt::Monday:
            tableModel = m_ui.tblMonday->model();
            listModel = m_ui.lvMondayTotals->model();
            break;

          case Qt::Tuesday:
            tableModel = m_ui.tblTuesday->model();
            listModel = m_ui.lvTuesdayTotals->model();
            break;

          case Qt::Wednesday:
            tableModel = m_ui.tblWednesday->model();
            listModel = m_ui.lvWednesdayTotals->model();
            break;

          case Qt::Thursday:
            tableModel = m_ui.tblThursday->model();
            listModel = m_ui.lvThursdayTotals->model();
            break;

          case Qt::Friday:
            tableModel = m_ui.tblFriday->model();
            listModel = m_ui.lvFridayTotals->model();
            break;

          case Qt::Saturday:
            tableModel = m_ui.tblSaturday->model();
            listModel = m_ui.lvSaturdayTotals->model();
            break;

          case Qt::Sunday:
            tableModel = m_ui.tblSunday->model();
            listModel = m_ui.lvSundayTotals->model();
            break;
        }
        /* Set cleanup to false because we don't want to delete old models */
        setupDay(m_ui.tblToday, m_ui.lvTodayTotals, tableModel, listModel, false);
        return;
      }
    }
  }

  /* Today */
  setupDay(m_ui.tblToday, m_ui.lvTodayTotals, today);
  todayTableModel = m_ui.tblToday->model();
  todayListModel = m_ui.lvTodayTotals->model();

  /* Monday */
  if (today.dayOfWeek() == Qt::Monday) {
    setupDay(m_ui.tblMonday, m_ui.lvMondayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblMonday, m_ui.lvMondayTotals, monday);
  }

  /* Tuesday */
  if (today.dayOfWeek() == Qt::Tuesday) {
    setupDay(m_ui.tblTuesday, m_ui.lvTuesdayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblTuesday, m_ui.lvTuesdayTotals, monday.addDays(1));
  }

  /* Wednesday */
  if (today.dayOfWeek() == Qt::Wednesday) {
    setupDay(m_ui.tblWednesday, m_ui.lvWednesdayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblWednesday, m_ui.lvWednesdayTotals, monday.addDays(2));
  }

  /* Thursday */
  if (today.dayOfWeek() == Qt::Thursday) {
    setupDay(m_ui.tblThursday, m_ui.lvThursdayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblThursday, m_ui.lvThursdayTotals, monday.addDays(3));
  }

  /* Friday */
  if (today.dayOfWeek() == Qt::Friday) {
    setupDay(m_ui.tblFriday, m_ui.lvFridayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblFriday, m_ui.lvFridayTotals, monday.addDays(4));
  }

  /* Saturday */
  if (today.dayOfWeek() == Qt::Saturday) {
    setupDay(m_ui.tblSaturday, m_ui.lvSaturdayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblSaturday, m_ui.lvSaturdayTotals, monday.addDays(5));
  }

  /* Sunday */
  if (today.dayOfWeek() == Qt::Sunday) {
    setupDay(m_ui.tblSunday, m_ui.lvSundayTotals, todayTableModel, todayListModel);
  }
  else {
    setupDay(m_ui.tblSunday, m_ui.lvSundayTotals, monday.addDays(6));
  }
}

void MainWindow::on_action_Quit_triggered()
{
  QApplication::quit();
}

void MainWindow::on_action_About_triggered()
{
  AboutDialog dialog(this);
  dialog.exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
  QApplication::aboutQt();
}

void MainWindow::on_action_Preferences_triggered()
{
  PreferencesDialog dialog(this);
  dialog.setModal(true);
  if (dialog.exec() == QDialog::Accepted) {
    if (m_trayIconAvailable && dialog.showTrayIcon() != m_showTrayIcon) {
      if (m_showTrayIcon) {
        m_trayIcon->hide();
        emit trayIconHidden();
      }
      else {
        if (m_trayIcon == NULL) {
          createTrayIcon();
        }
        else {
          m_trayIcon->show();
          emit trayIconShown();
        }
      }
      m_showTrayIcon = !m_showTrayIcon;
    }
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
  ActivityDialog *dialog = new ActivityDialog(this);
  dialog->setActivityCompleter(m_activityCompleter);
  dialog->setTagCompleter(m_tagCompleter);
  dialog->setModal(true);
  if (dialog->exec() == QDialog::Accepted) {
    QSharedPointer<Activity> activity = dialog->activity();
    if (activity->isRunning()) {
      stopCurrentActivities();
    }
    activity->save();

    m_recordManager->addRecordPointer(activity);
    emit activityCreated(activity);
    refreshCompleterModels();
  }
  dialog->deleteLater();
}

void MainWindow::editActivity(QSharedPointer<Activity> activity)
{
  bool wasRunning = activity->isRunning();

  ActivityDialog *dialog = new ActivityDialog(activity, this);
  dialog->setActivityCompleter(m_activityCompleter);
  dialog->setTagCompleter(m_tagCompleter);
  dialog->setModal(true);
  if (dialog->exec() == QDialog::Accepted) {
    if (!wasRunning && activity->isRunning()) {
      stopCurrentActivities();
    }
    activity->save();
  }
  dialog->deleteLater();
}

void MainWindow::startActivityLike(QSharedPointer<Activity> activity)
{
  Activity *newActivity = Activity::startLike(activity.data());
  if (newActivity != NULL) {
    stopCurrentActivities();
    newActivity->save();

    QSharedPointer<Activity> ptr = m_recordManager->addRecord(newActivity);
    emit activityCreated(ptr);
    refreshCompleterModels();
  }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger) {
    if (isVisible()) {
      hide();
    }
    else {
      showNormal();
    }
  }
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
    stopCurrentActivities();
    activity->save();

    QSharedPointer<Activity> ptr = m_recordManager->addRecord(activity);
    emit activityCreated(ptr);
    refreshCompleterModels();
  }
  else {
    activity->deleteLater();
  }
}

void MainWindow::stopCurrentActivities()
{
  if (m_settings.value("noConcurrentActivities", true).toBool()) {
    QList<int> activityIds = Activity::findCurrentIds();
    for (int i = 0; i < activityIds.count(); i++) {
      QSharedPointer<Activity> activity = m_recordManager->getRecordById(activityIds[i]);
      activity->stop();
    }
  }
}

void MainWindow::setupDay(ActivityTableView *tableView, ProjectTotalsListView *listView, const QDate &date)
{
  /* Setup table model */
  ActivityTableModel *tableModel = new ActivityTableModel(date, m_recordManager, this);
  connect(this, SIGNAL(activityCreated(QSharedPointer<Activity>)),
      tableModel, SLOT(created(QSharedPointer<Activity>)));
  connect(tableModel, SIGNAL(activityStarted()),
      m_ui.tblCurrent->model(), SLOT(refreshActivities()));
  connect(tableModel, SIGNAL(activitySaved()),
      m_activityCompleterModel, SLOT(refreshNames()));
  connect(tableModel, SIGNAL(activityDestroyed()),
      m_activityCompleterModel, SLOT(refreshNames()));
  connect(tableModel, SIGNAL(activitySaved()),
      m_tagCompleterModel, SLOT(refreshNames()));
  connect(tableModel, SIGNAL(activityDestroyed()),
      m_tagCompleterModel, SLOT(refreshNames()));

  /* Setup list model */
  ProjectTotalsListModel *listModel = new ProjectTotalsListModel(date, m_recordManager, this);
  connect(this, SIGNAL(activityCreated(QSharedPointer<Activity>)),
      listModel, SLOT(created(QSharedPointer<Activity>)));

  setupDay(tableView, listView, tableModel, listModel);
}

void MainWindow::setupDay(ActivityTableView *tableView, ProjectTotalsListView *listView, AbstractActivityModel *tableModel, AbstractActivityModel *listModel, bool cleanup)
{
  AbstractActivityModel *oldTableModel = tableView->model();
  AbstractActivityModel *oldListModel = listView->model();

  /* Set table model */
  tableView->setModel(tableModel);
  ActivityDelegate *tableDelegate = qobject_cast<ActivityDelegate *>(tableView->itemDelegate());
  if (tableDelegate == NULL) {
    /* Only set delegate if it hasn't already been set */
    tableView->setItemDelegate(new ActivityDelegate(tableView));
  }

  /* Disconnect the view first so we don't double connect signals on 
   * subsequent calls */
  disconnect(tableView, 0, this, 0);
  connect(tableView, SIGNAL(editActivity(QSharedPointer<Activity>)),
      SLOT(editActivity(QSharedPointer<Activity>)));
  connect(tableView, SIGNAL(startActivityLike(QSharedPointer<Activity>)),
      SLOT(startActivityLike(QSharedPointer<Activity>)));

  /* Set list model */
  listView->setModel(listModel);
  ProjectTotalDelegate *listDelegate = qobject_cast<ProjectTotalDelegate *>(listView->itemDelegate());
  if (listDelegate == NULL) {
    listView->setItemDelegate(new ProjectTotalDelegate(listView));
  }

  /* Now, if there are old models, clean them up */
  if (cleanup) {
    if (oldTableModel != NULL) {
      disconnect(this, 0, oldTableModel, 0);
      disconnect(oldTableModel, 0, m_ui.tblCurrent->model(), 0);
      disconnect(oldTableModel, 0, m_activityCompleterModel, 0);
      disconnect(oldTableModel, 0, m_tagCompleterModel, 0);
      oldTableModel->deleteLater();
    }

    if (oldListModel != NULL) {
      disconnect(this, 0, oldListModel, 0);
      oldListModel->deleteLater();
    }
  }
}

void MainWindow::createTrayIcon()
{
  m_trayIconMenu = new QMenu(this);
  m_trayIconMenu->setTitle("Hourglass");

  m_trayIconMenu->addAction(m_ui.action_Quit);

  m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/hourglass.png"), this);
  connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
      this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

  /* Not sure what this is, but it's in the Qt example :) */
  QByteArray category = qgetenv("SNI_CATEGORY");
  if (!category.isEmpty()) {
    m_trayIcon->setProperty("_qt_sni_category", QString::fromLocal8Bit(category));
  }

  m_trayIcon->setContextMenu(m_trayIconMenu);
  m_trayIcon->show();
  emit trayIconShown();
}

void MainWindow::refreshCompleterModels()
{
  m_activityCompleterModel->refreshNames();
  m_tagCompleterModel->refreshNames();
}
