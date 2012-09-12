#ifndef __ACTIVITYTABLEVIEW_H
#define __ACTIVITYTABLEVIEW_H

#include <QTableView>
#include <QDate>

class ActivityTableView : public QTableView
{
  Q_OBJECT

  public:
    ActivityTableView(QWidget *parent = 0);

    void setDate(const QDate &date);
    void setModel(QAbstractItemModel *model);
};

#endif
