#ifndef __ACTIVITYNAMESLISTMODEL_H
#define __ACTIVITYNAMESLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class ActivityNamesListModel : public QAbstractListModel
{
  Q_OBJECT

  public:
    ActivityNamesListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  public slots:
    void refreshNames();

  private:
    QStringList m_names;

    QStringList fetchNames() const;
};

#endif
