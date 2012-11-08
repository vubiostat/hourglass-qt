#ifndef __RECORDMANAGER_H
#define __RECORDMANAGER_H

#include <QHash>
#include <QWeakPointer>
#include <QSharedPointer>

template <class T>
class RecordManager
{
  public:
    QSharedPointer<T> getRecordById(int id)
    {
      QSharedPointer<T> ptr;
      if (m_records.contains(id)) {
        ptr = m_records[id].toStrongRef();
      }

      if (ptr.isNull()) {
        ptr = addRecord(T::findById(id));
      }
      return ptr;
    }

    QSharedPointer<T> addRecord(T *record)
    {
      QSharedPointer<T> ptr(record, &QObject::deleteLater);
      m_records[record->id()] = ptr.toWeakRef();
      return ptr;
    }

    void addRecordPointer(QSharedPointer<T> ptr)
    {
      m_records[ptr->id()] = ptr.toWeakRef();
    }

  private:
    QHash<int, QWeakPointer<T> > m_records;
};

#endif
