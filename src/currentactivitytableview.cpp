#include "currentactivitytableview.h"
#include "currentactivitydelegate.h"

CurrentActivityTableView::CurrentActivityTableView(QWidget *parent)
  : ActivityTableView(parent)
{
}

CurrentActivityTableModel *CurrentActivityTableView::model() const
{
  return static_cast<CurrentActivityTableModel *>(ActivityTableView::model());
}

void CurrentActivityTableView::addWidgets()
{
  while (!m_stopButtons.isEmpty()) {
    m_stopButtons.takeLast()->deleteLater();
  }

  ActivityTableModel *m = model();
  for (int i = 0; i < m->rowCount(); i++) {
    QSharedPointer<Activity> activity = m->activityAt(i);
    QPushButton *stopButton = new QPushButton("Stop activity", this);
    connect(stopButton, SIGNAL(clicked()), activity.data(), SLOT(stop()));
    setIndexWidget(m->index(i, 4), stopButton);
    m_stopButtons.append(stopButton);
  }
}
