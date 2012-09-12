#include "mainwindow.h"
#include "activitydelegate.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
{
  m_ui.setupUi(this);

  m_activityTableModel = new ActivityTableModel(this);
  m_ui.tblToday->setModel(m_activityTableModel);
  m_ui.tblToday->setItemDelegate(new ActivityDelegate(this));

  QHeaderView *headerView = m_ui.tblToday->horizontalHeader();
  headerView->setResizeMode(QHeaderView::ResizeToContents);
  headerView->setResizeMode(5, QHeaderView::Stretch);
}
