#include "currentactivitytableview.h"
#include "currentactivitydelegate.h"

CurrentActivityTableView::CurrentActivityTableView(QWidget *parent)
  : ActivityTableView(parent)
{
  verticalHeader()->setDefaultSectionSize(35);
}

CurrentActivityTableModel *CurrentActivityTableView::model() const
{
  return static_cast<CurrentActivityTableModel *>(ActivityTableView::model());
}

const QSize &CurrentActivityTableView::stopButtonSize() const
{
  return m_stopButtonSize;
}

void CurrentActivityTableView::setStopButtonSize(const QSize &size)
{
  m_stopButtonSize = size;
  horizontalHeader()->resizeSection(4, size.width());
}

void CurrentActivityTableView::setHeaderStretch()
{
  horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
  horizontalHeader()->setResizeMode(4, QHeaderView::Fixed);
}

void CurrentActivityTableView::clicked(const QModelIndex &index)
{
  switch (index.column()) {
    case 4:
      /* Stop button */
      if (index.data().toInt() >= 0) {
        model()->activityAt(index.row())->stop();
      }
      break;
  }
}

void CurrentActivityTableView::doubleClicked(const QModelIndex &index)
{
  /* Do nothing */
}
