#include "projecttotalslistview.h"

ProjectTotalsListView::ProjectTotalsListView(QWidget *parent)
  : QListView(parent)
{
  /* Set transparent background */
  QPalette pal(palette());
  pal.setColor(QPalette::Base, Qt::transparent);
  setPalette(pal);

  setFlow(QListView::LeftToRight);
  setFrameStyle(QFrame::NoFrame | QFrame::Plain);
  setSelectionMode(QAbstractItemView::NoSelection);
}

AbstractActivityModel *ProjectTotalsListView::model() const
{
  return static_cast<AbstractActivityModel *>(QListView::model());
}

void ProjectTotalsListView::setModel(AbstractActivityModel *newModel)
{
  if (model()) {
    disconnect(model(), 0, this, 0);
  }

  QListView::setModel((QAbstractItemModel *) newModel);

  if (newModel) {
    newModel->refreshActivities();
    connect(newModel, SIGNAL(modelReset()), this, SLOT(modelReset()));

    resizeHeightIfFixed();
  }
}

QSize ProjectTotalsListView::minimumSizeHint() const
{
  QSize size(QListView::sizeHint());
  if (model()) {
    /* Assume flow is LeftToRight and take the maximum height */
    int height = 0;
    int rowCount = model()->rowCount();
    for (int i = 0; i < rowCount; i++) {
      height = qMax(height, sizeHintForRow(i));
    }
    size.setHeight(height + 4);
  }
  return size;
}

void ProjectTotalsListView::modelReset()
{
  resizeHeightIfFixed();
}

void ProjectTotalsListView::resizeHeightIfFixed()
{
  if (sizePolicy().verticalPolicy() == QSizePolicy::Fixed) {
    setFixedHeight(minimumSizeHint().height());
  }
}
