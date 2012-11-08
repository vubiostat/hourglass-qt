#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QCompleter>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QTimer>
#include "ui_mainwindow.h"

#include "hourglass/recordmanager.h"
#include "hourglass/activity.h"
#include "hourglass/activitynameslistmodel.h"
#include "hourglass/tagnameslistmodel.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    bool isTrayIconShown() const;

  signals:
    void activityCreated(QSharedPointer<Activity> activity);
    void trayIconHidden();
    void trayIconShown();

  protected:
    void paintEvent(QPaintEvent *event);

  private slots:
    void setupViews();
    void on_action_Quit_triggered();
    void on_action_About_triggered();
    void on_actionAbout_Qt_triggered();
    void on_action_Preferences_triggered();
    void on_btnStartActivity_clicked();
    void on_leActivity_returnPressed();
    void on_leTags_returnPressed();
    void on_btnAddEarlierActivity_clicked();
    void editActivity(QSharedPointer<Activity> activity);
    void startActivityLike(QSharedPointer<Activity> activity);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

  private:
    Ui::MainWindow m_ui;
    RecordManager<Activity> *m_recordManager;

    QSize m_startButtonSize;

    QCompleter *m_activityCompleter;
    QCompleter *m_tagCompleter;
    ActivityNamesListModel *m_activityCompleterModel;
    TagNamesListModel *m_tagCompleterModel;

    QSettings m_settings;

    bool m_trayIconAvailable;
    bool m_showTrayIcon;
    QMenu *m_trayIconMenu;
    QSystemTrayIcon *m_trayIcon;

    QTimer *m_dayTimer;

    void startActivity();
    void stopCurrentActivities();
    void setupDay(ActivityTableView *tableView, ProjectTotalsListView *listView, const QDate &date);
    void setupDay(ActivityTableView *tableView, ProjectTotalsListView *listView, AbstractActivityModel *tableModel, AbstractActivityModel *listModel, bool cleanup = true);
    void createTrayIcon();
    void refreshCompleterModels();
};

#endif
