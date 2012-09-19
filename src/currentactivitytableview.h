#ifndef __CURRENTACTIVITYTABLEVIEW_H
#define __CURRENTACTIVITYTABLEVIEW_H

#include "activitytableview.h"
#include "currentactivitytablemodel.h"

class CurrentActivityTableView : public ActivityTableView
{
  Q_OBJECT

  public:
    CurrentActivityTableView(QWidget *parent = 0);

    CurrentActivityTableModel *model() const;

  protected:
    void addWidgets();

  private:
    QList<QPushButton *> m_stopButtons;
};

#endif
