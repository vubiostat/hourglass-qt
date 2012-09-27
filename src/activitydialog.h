#ifndef __ACTIVITYDIALOG_H
#define __ACTIVITYDIALOG_H

#include <QDialog>
#include <QSharedPointer>
#include <QPalette>
#include "ui_activitydialog.h"
#include "activity.h"

class ActivityDialog : public QDialog
{
  Q_OBJECT

  public:
    ActivityDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ActivityDialog(QSharedPointer<Activity> activity, QWidget *parent = 0, Qt::WindowFlags f = 0);

    QSharedPointer<Activity> activity() const;
    void setActivityCompleter(QCompleter *completer);
    void setTagCompleter(QCompleter *completer);

  public slots:
    void accept();

  private slots:
    void on_cbInProgress_clicked();
    void on_deStartedAt_dateChanged(const QDate &date);
    void on_teStartedAt_timeChanged(const QTime &time);
    void on_deEndedAt_dateChanged(const QDate &date);
    void on_teEndedAt_timeChanged(const QTime &time);

  private:
    Ui::ActivityDialog m_ui;
    QSharedPointer<Activity> m_activity;
    QPalette m_frStartedAtPalette;
    QPalette m_frEndedAtPalette;
    bool m_errorStartedAt;
    bool m_errorEndedAt;

    void setupFrames();
    QDateTime startedAt();
    QDateTime endedAt();
    bool isTimeValid();
    void setErrorStartedAt(bool toggle);
    void setErrorEndedAt(bool toggle);
};

#endif
