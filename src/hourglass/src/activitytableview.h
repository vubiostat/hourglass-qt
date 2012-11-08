#ifndef __ACTIVITYTABLEVIEW_H
#define __ACTIVITYTABLEVIEW_H

#include <QTableView>
#include <QDate>
#include <QList>
#include <QPushButton>
#include <QHeaderView>
#include "hourglass/abstractactivitymodel.h"

class ActivityTableView : public QTableView
{
  Q_OBJECT

  public:
    ActivityTableView(QWidget *parent = 0);

    AbstractActivityModel *model() const;
    void setModel(AbstractActivityModel *model);

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
    virtual void checkAllUntimed();

  private:
    void resizeHeightIfFixed();
};

#endif
