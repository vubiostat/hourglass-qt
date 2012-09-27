#include "currentactivitytableview.h"
#include "currentactivitydelegate.h"

CurrentActivityTableView::CurrentActivityTableView(QWidget *parent)
  : ActivityTableView(parent)
{
  verticalHeader()->setDefaultSectionSize(35);
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
  horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
  horizontalHeader()->setResizeMode(4, QHeaderView::Fixed);
}

void CurrentActivityTableView::checkAllUntimed()
{
  /* Do nothing */
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
  Q_UNUSED(index);
}
