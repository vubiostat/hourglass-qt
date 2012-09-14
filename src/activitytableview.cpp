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

ActivityTableModel *ActivityTableView::model() const
{
  return static_cast<ActivityTableModel *>(QTableView::model());
}

void ActivityTableView::setModel(ActivityTableModel *newModel)
{
  if (model()) {
    disconnect(model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(rowsInserted(const QModelIndex &, int, int)));
    disconnect(model(), SIGNAL(columnsInserted(const QModelIndex &, int, int)),
        this, SLOT(columnsInserted(const QModelIndex &, int, int)));
    disconnect(model(), SIGNAL(modelReset()),
        this, SLOT(modelReset()));
  }

  QTableView::setModel((QAbstractItemModel *) newModel);

  if (newModel) {
    connect(newModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(rowsInserted(const QModelIndex &, int, int)));
    connect(newModel, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
        this, SLOT(columnsInserted(const QModelIndex &, int, int)));
    connect(newModel, SIGNAL(modelReset()),
        this, SLOT(modelReset()));
    horizontalHeader()->setResizeMode(5, QHeaderView::Stretch);

    resizeHeightIfFixed();
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
    size.setHeight(height + (rowCount * 5));
  }

  return size;
}

void ActivityTableView::rowsInserted(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent); Q_UNUSED(start); Q_UNUSED(end);
  resizeHeightIfFixed();
}

void ActivityTableView::columnsInserted(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent); Q_UNUSED(start); Q_UNUSED(end);
  resizeHeightIfFixed();
}

void ActivityTableView::modelReset()
{
  resizeHeightIfFixed();
}

void ActivityTableView::resizeHeightIfFixed()
{
  if (sizePolicy().verticalPolicy() == QSizePolicy::Fixed) {
    setFixedHeight(minimumSizeHint().height());
  }
}
