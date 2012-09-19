#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "recordmanager.h"
#include "activity.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

  signals:
    void activityCreated(QSharedPointer<Activity> activity);

  private slots:
    void on_btnStartActivity_clicked();
    void on_leActivity_returnPressed();
    void on_leTags_returnPressed();
    void on_btnAddEarlierActivity_clicked();
    void editActivity(QSharedPointer<Activity> activity);

  private:
    Ui::MainWindow m_ui;
    RecordManager<Activity> *m_recordManager;

    void startActivity();
    void setupActivityTableView(ActivityTableView *view, const QDate &date);
    void setupActivityTableView(ActivityTableView *view, ActivityTableModel *model);
};

#endif
