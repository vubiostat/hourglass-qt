#ifndef __CURRENTACTIVITYTABLEVIEW_H
#define __CURRENTACTIVITYTABLEVIEW_H

#include "activitytableview.h"
#include "currentactivitytablemodel.h"
#include <QSize>

class CurrentActivityTableView : public ActivityTableView
{
  Q_OBJECT

  public:
    CurrentActivityTableView(QWidget *parent = 0);

    CurrentActivityTableModel *model() const;
    const QSize &stopButtonSize() const;
    void setStopButtonSize(const QSize &size);

  protected:
    void setHeaderStretch();

  protected slots:
    void clicked(const QModelIndex &index);
    void doubleClicked(const QModelIndex &index);

  private:
    QSize m_stopButtonSize;
};

#endif
