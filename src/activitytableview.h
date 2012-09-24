#ifndef __ACTIVITYTABLEVIEW_H
#define __ACTIVITYTABLEVIEW_H

#include <QTableView>
#include <QDate>
#include <QList>
#include <QPushButton>
#include <QHeaderView>
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
    void startActivityLike(QSharedPointer<Activity> activity);

  protected slots:
    void modelReset();
    virtual void clicked(const QModelIndex &index);
    virtual void doubleClicked(const QModelIndex &index);

  protected:
    virtual void setHeaderStretch();

  private:
    void resizeHeightIfFixed();
};

#endif
