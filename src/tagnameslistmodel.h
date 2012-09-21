#ifndef __TAGNAMESLISTMODEL_H
#define __TAGNAMESLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class TagNamesListModel : public QAbstractListModel
{
  Q_OBJECT

  public:
    TagNamesListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  public slots:
    void refreshNames();

  private:
    QStringList m_names;

    QStringList fetchNames() const;
};

#endif
