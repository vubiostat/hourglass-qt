#include "activitytableview.h"
#include <QHeaderView>
#include <QIcon>
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
}

ActivityTableModel *ActivityTableView::model() const
{
  return static_cast<ActivityTableModel *>(QTableView::model());
}

void ActivityTableView::setModel(ActivityTableModel *newModel)
{
  if (model()) {
    disconnect(model(), 0, this, 0);
  }

  QTableView::setModel((QAbstractItemModel *) newModel);

  if (newModel) {
    newModel->refreshActivities();
    connect(newModel, SIGNAL(modelReset()), this, SLOT(modelReset()));
    horizontalHeader()->setResizeMode(5, QHeaderView::Stretch);

    addWidgets();
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
    size.setHeight(height + (rowCount * 4));
  }

  return size;
}

void ActivityTableView::modelReset()
{
  addWidgets();
  resizeHeightIfFixed();
}

void ActivityTableView::addWidgets()
{
  while (!m_editButtons.isEmpty()) {
    m_editButtons.takeLast()->deleteLater();
  }
  while (!m_deleteButtons.isEmpty()) {
    m_deleteButtons.takeLast()->deleteLater();
  }

  ActivityTableModel *m = model();
  for (int i = 0; i < m->rowCount(); i++) {
    QPushButton *editButton = new QPushButton(this);
    editButton->setIcon(QIcon::fromTheme("accessories-text-editor"));
    editButton->setFlat(true);
    connect(editButton, SIGNAL(clicked()), SLOT(editButtonClicked()));
    setIndexWidget(m->index(i, 7), editButton);
    m_editButtons.append(editButton);

    QPushButton *deleteButton = new QPushButton(this);
    deleteButton->setIcon(QIcon::fromTheme("user-trash"));
    deleteButton->setFlat(true);
    connect(deleteButton, SIGNAL(clicked()), SLOT(deleteButtonClicked()));
    setIndexWidget(m->index(i, 8), deleteButton);
    m_deleteButtons.append(deleteButton);
  }
}

void ActivityTableView::editButtonClicked()
{
  int index = m_editButtons.indexOf(static_cast<QPushButton *>(QObject::sender()));
  if (index >= 0) {
    QSharedPointer<Activity> activity = model()->activityAt(index);
    emit editActivity(activity);
  }
}

void ActivityTableView::deleteButtonClicked()
{
  int index = m_deleteButtons.indexOf(static_cast<QPushButton *>(QObject::sender()));
  if (index >= 0) {
    QMessageBox::StandardButton questionResult = QMessageBox::question(this,
        "Delete activity", "Are you sure you want to delete the activity?",
        QMessageBox::Yes | QMessageBox::No);
    if (questionResult == QMessageBox::Yes) {
      QSharedPointer<Activity> activity = model()->activityAt(index);
      activity->destroy();
    }
  }
}

void ActivityTableView::resizeHeightIfFixed()
{
  if (sizePolicy().verticalPolicy() == QSizePolicy::Fixed) {
    setFixedHeight(minimumSizeHint().height());
  }
}
