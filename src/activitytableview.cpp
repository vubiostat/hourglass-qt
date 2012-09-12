#include "activitytableview.h"
#include "activitytablemodel.h"
#include "activitydelegate.h"
#include <QHeaderView>

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

  setItemDelegate(new ActivityDelegate(this));
}

void ActivityTableView::setDate(const QDate &date)
{
  ActivityTableModel *newModel = new ActivityTableModel(date, this);
  setModel(newModel);
}

void ActivityTableView::setModel(QAbstractItemModel *model)
{
  QTableView::setModel(model);

  if (model) {
    horizontalHeader()->setResizeMode(5, QHeaderView::Stretch);
  }
}
