#ifndef __ACTIVITYTABLEVIEW_H
#define __ACTIVITYTABLEVIEW_H

#include <QTableView>
#include <QDate>
#include <QList>
#include <QPushButton>
#include "activitytablemodel.h"

class ActivityTableView : public QTableView
{
  Q_OBJECT

  public:
    ActivityTableView(QWidget *parent = 0);

    ActivityTableModel *model() const;
    void setModel(ActivityTableModel *model);

    QSize minimumSizeHint() const;

  signals:
    void editActivity(QSharedPointer<Activity> activity);

  protected slots:
    void modelReset();

  protected:
    virtual void addWidgets();

  private slots:
    void editButtonClicked();
    void deleteButtonClicked();

  private:
    void resizeHeightIfFixed();
    QList<QPushButton *> m_editButtons;
    QList<QPushButton *> m_deleteButtons;
};

#endif
