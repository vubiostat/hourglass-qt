#include "mainwindow.h"
#include "activitydialog.h"
#include "activity.h"
#include "currentactivitytablemodel.h"
#include "activitytablemodel.h"
#include "activitydelegate.h"
#include "currentactivitydelegate.h"
#include "namesdelegate.h"
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
      m_ui.tblCurrent->model(), SLOT(activityCreated(QSharedPointer<Activity>)));

  QDate today = QDate::currentDate();
  QDate sunday;
  if (today.dayOfWeek() == Qt::Sunday) {
    sunday = today;
  }
  else {
    sunday = today.addDays(-today.dayOfWeek());
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

void MainWindow::paintEvent(QPaintEvent *event)
{
  QMainWindow::paintEvent(event);

  QSize size = m_ui.btnStartActivity->size();
  if (m_startButtonSize != size) {
    m_startButtonSize = size;
    m_ui.tblCurrent->setStopButtonSize(size);
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

void MainWindow::setupActivityTableView(ActivityTableView *view, const QDate &date)
{
  ActivityTableModel *model = new ActivityTableModel(date, m_recordManager, this);
  connect(this, SIGNAL(activityCreated(QSharedPointer<Activity>)),
      model, SLOT(activityCreated(QSharedPointer<Activity>)));
  connect(model, SIGNAL(activityStarted()),
      m_ui.tblCurrent->model(), SLOT(refreshActivities()));
  connect(model, SIGNAL(activitySaved()),
      m_activityCompleterModel, SLOT(refreshNames()));
  connect(model, SIGNAL(activityDestroyed()),
      m_activityCompleterModel, SLOT(refreshNames()));
  connect(model, SIGNAL(activitySaved()),
      m_tagCompleterModel, SLOT(refreshNames()));
  connect(model, SIGNAL(activityDestroyed()),
      m_tagCompleterModel, SLOT(refreshNames()));
  setupActivityTableView(view, model);
}

void MainWindow::setupActivityTableView(ActivityTableView *view, ActivityTableModel *model)
{
  view->setModel(model);
  view->setItemDelegate(new ActivityDelegate(view));
  connect(view, SIGNAL(editActivity(QSharedPointer<Activity>)),
      SLOT(editActivity(QSharedPointer<Activity>)));
  connect(view, SIGNAL(startActivityLike(QSharedPointer<Activity>)),
      SLOT(startActivityLike(QSharedPointer<Activity>)));
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
