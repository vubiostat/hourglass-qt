#include "activitytableview.h"
#include <QMessageBox>

ActivityTableView::ActivityTableView(QWidget *parent)
  : QTableView(parent)
{
  setShowGrid(false);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  horizontalHeader()->setMinimumSectionSize(0);
  horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  horizontalHeader()->hide();
  verticalHeader()->hide();

  connect(this, SIGNAL(clicked(const QModelIndex &)),
      this, SLOT(clicked(const QModelIndex &)));
  connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
      this, SLOT(doubleClicked(const QModelIndex &)));
}

AbstractActivityModel *ActivityTableView::model() const
{
  return static_cast<AbstractActivityModel *>(QTableView::model());
}

void ActivityTableView::setModel(AbstractActivityModel *newModel)
{
  if (model()) {
    disconnect(model(), 0, this, 0);
  }

  QTableView::setModel((QAbstractItemModel *) newModel);

  if (newModel) {
    newModel->refreshActivities();
    connect(newModel, SIGNAL(modelReset()), this, SLOT(modelReset()));

    setHeaderStretch();
    resizeHeightIfFixed();
    checkAllUntimed();
  }
}

QSize ActivityTableView::minimumSizeHint() const
{
  QSize size(QTableView::sizeHint());

  if (model()) {
    int height = horizontalHeader()->height();
    int rowCount = model()->rowCount();
    for (int i = 0; i < rowCount; i++) {
      height += rowHeight(i);
    }
    size.setHeight(height + (rowCount * 4));
  }

  return size;
}

void ActivityTableView::modelReset()
{
  resizeHeightIfFixed();
  checkAllUntimed();
}

void ActivityTableView::clicked(const QModelIndex &index)
{
  switch (index.column()) {
    case 7:
      /* Edit icon */
      emit editActivity(model()->activityAt(index.row()));
      break;

    case 8:
      /* Delete icon */
      QMessageBox::StandardButton questionResult = QMessageBox::question(this,
          "Delete activity", "Are you sure you want to delete the activity?",
          QMessageBox::Yes | QMessageBox::No);
      if (questionResult == QMessageBox::Yes) {
        QSharedPointer<Activity> activity = model()->activityAt(index.row());
        activity->destroy();
      }
      break;
  }
}

void ActivityTableView::doubleClicked(const QModelIndex &index)
{
  if (index.column() < 7) {
    emit startActivityLike(model()->activityAt(index.row()));
  }
}

void ActivityTableView::setHeaderStretch()
{
  horizontalHeader()->setResizeMode(5, QHeaderView::Stretch);
}

void ActivityTableView::checkAllUntimed()
{
  bool hide = model()->allUntimed();
  setColumnHidden(0, hide);
  setColumnHidden(1, hide);
}

void ActivityTableView::resizeHeightIfFixed()
{
  if (sizePolicy().verticalPolicy() == QSizePolicy::Fixed) {
    setFixedHeight(minimumSizeHint().height());
  }
}
