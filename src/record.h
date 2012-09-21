#ifndef __RECORD_H
#define __RECORD_H

#include <QtDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlRecord>

template<typename T>
struct is_pointer { static const bool value = false; };

template<typename T>
struct is_pointer<T*> { static const bool value = true; };

class Record : public QObject
{
  Q_OBJECT

  public:

    template <class T>
    static QList<T*> find(const QString &tableName, QObject *parent = 0)
    {
      return find<T>(tableName, QString(), parent);
    }

    template <class T>
    static QList<T*> find(const QString &tableName, const QString &conditions, QObject *parent = 0)
    {
      QList<QVariant> emptyBindValues;
      return find<T>(tableName, conditions, emptyBindValues, parent);
    }

    template <class T>
    static QList<T*> find(const QString &tableName, const QString &conditions, const QString &predicate, QObject *parent = 0)
    {
      QList<QVariant> emptyBindValues;
      return find<T>(tableName, conditions, emptyBindValues, predicate, parent);
    }

    template <class T>
    static T* findById(const QString &tableName, int id, QObject *parent = 0)
    {
      QList<QVariant> bindValues;
      bindValues << QVariant(id);
      QList<T*> records = find<T>(tableName, "WHERE id = ?", bindValues, parent);
      T *result = NULL;
      for (int i = 0; i < records.count(); i++) {
        if (i == 0) {
          result = records[i];
        }
        else {
          records[i]->deleteLater();
        }
      }
      return result;
    }

    template <class T>
    static QList<T*> find(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues, QObject *parent = 0)
    {
      return find<T>(tableName, conditions, bindValues, QString(), parent);
    }

    template <class T>
    static QList<T*> find(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate, QObject *parent = 0)
    {
      QSqlQuery query;
      if (!executeFindQuery(query, tableName, "*", conditions, bindValues, predicate)) {
        qDebug() << "Query failed:" << query.lastQuery();
        return QList<T*>();
      }

      QList<T*> result;
      while (query.next()) {
        QSqlRecord record = query.record();
        QMap<QString, QVariant> attributes;
        for (int i = 0; i < record.count(); i++) {
          attributes.insert(record.fieldName(i), query.value(i));
        }

        result << new T(attributes, false, parent);
      }
      return result;
    }

    static QList<int> findIds(const QString &tableName);
    static QList<int> findIds(const QString &tableName, const QString &conditions);
    static QList<int> findIds(const QString &tableName, const QString &conditions, const QString &predicate);
    static QList<int> findIds(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues);
    static QList<int> findIds(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate);

    static int count(const QString &tableName);
    static int count(const QString &tableName, const QString &conditions);
    static int count(const QString &tableName, const QString &conditions, const QList<QVariant> &bindValues);

    Record(QObject *parent = 0);
    Record(const Record &other);
    Record &operator=(const Record &other);

    QVariant get(const QString &attributeName, bool dirty = true) const;
    void set(const QString &attributeName, const QVariant &value);
    void unset(const QString &attributeName);
    bool containsAttribute(const QString &attributeName) const;

    int id() const;
    bool isNew() const;
    bool wasNew() const;
    bool isModified() const;
    bool isValid();
    bool save(const QString &tableName);
    bool destroy(const QString &tableName);
    void reset();

    bool operator==(const Record &other);

  signals:
    void saved();
    void destroyed();

  protected:
    Record(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);
    static QSqlDatabase database();

    virtual void beforeValidation();
    virtual bool validate();
    virtual void beforeCreate();
    virtual void beforeSave();
    virtual void afterSave();
    virtual void afterCreate();

  private:
    static bool executeFindQuery(QSqlQuery &query, const QString &tableName, const QString &select, const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate);

    QMap<QString, QVariant> attributes;
    QMap<QString, QVariant> dirtyAttributes;
    bool newRecord, modified, wasNewRecord;
};

#endif
