#ifndef __PROJECTTOTALSLISTVIEW_H
#define __PROJECTTOTALSLISTVIEW_H

#include <QListView>
#include "hourglass/abstractactivitymodel.h"

class ProjectTotalsListView : public QListView
{
  Q_OBJECT

  public:
    ProjectTotalsListView(QWidget *parent = 0);

    AbstractActivityModel *model() const;
    void setModel(AbstractActivityModel *model);

    QSize minimumSizeHint() const;

  protected slots:
    void modelReset();

  private:
    void resizeHeightIfFixed();
};

#endif
