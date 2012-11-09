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

class Record : public QObject
{
  Q_OBJECT

  public:
    static QSqlDatabase &database();
    static void setDatabase(QSqlDatabase &database);
    static const QString &classTableName();

    template <class T>
    static QList<T*> find(QObject *parent = 0)
    {
      return find<T>(QString(), parent);
    }

    template <class T>
    static QList<T*> find(const QString &conditions, QObject *parent = 0)
    {
      QList<QVariant> emptyBindValues;
      return find<T>(conditions, emptyBindValues, parent);
    }

    template <class T>
    static QList<T*> find(const QString &conditions, const QString &predicate, QObject *parent = 0)
    {
      QList<QVariant> emptyBindValues;
      return find<T>(conditions, emptyBindValues, predicate, parent);
    }

    template <class T>
    static T* findById(int id, QObject *parent = 0)
    {
      QList<QVariant> bindValues;
      bindValues << QVariant(id);
      QList<T*> records = find<T>("WHERE id = ?", bindValues, parent);
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
    static QList<T*> find(const QString &conditions, const QList<QVariant> &bindValues, QObject *parent = 0)
    {
      return find<T>(conditions, bindValues, QString(), parent);
    }

    template <class T>
    static QList<T*> find(const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate, QObject *parent = 0)
    {
      QSqlQuery query;
      if (!executeFindQuery(query, T::classTableName(), "*", conditions, bindValues, predicate)) {
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

    template <class T>
    static QList<int> findIds()
    {
      return findIds<T>(QString());
    }

    template <class T>
    static QList<int> findIds(const QString &conditions)
    {
      QList<QVariant> emptyBindValues;
      return findIds<T>(conditions, emptyBindValues);
    }

    template <class T>
    static QList<int> findIds(const QString &conditions, const QString &predicate)
    {
      QList<QVariant> emptyBindValues;
      return findIds<T>(conditions, emptyBindValues, predicate);
    }

    template <class T>
    static QList<int> findIds(const QString &conditions, const QList<QVariant> &bindValues)
    {
      return findIds<T>(conditions, bindValues, QString());
    }

    template <class T>
    static QList<int> findIds(const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate)
    {
      QSqlQuery query;
      if (!executeFindQuery(query, T::classTableName(), "id", conditions, bindValues, predicate)) {
        qDebug() << "Query failed:" << query.lastQuery();
        return QList<int>();
      }

      QList<int> result;
      while (query.next()) {
        result.append(query.value(0).toInt());
      }
      return result;
    }

    template <class T>
    static int count()
    {
      return count<T>(QString());
    }

    template <class T>
    static int count(const QString &conditions)
    {
      QList<QVariant> emptyBindValues;
      return count<T>(conditions, emptyBindValues);
    }

    template <class T>
    static int count(const QString &conditions, const QList<QVariant> &bindValues)
    {
      QStringList queryStrings;
      queryStrings << "SELECT COUNT(*) FROM";
      queryStrings << T::classTableName();
      if (!conditions.isEmpty()) {
        queryStrings << conditions;
      }
      QString queryString = queryStrings.join(" ");

      QSqlDatabase database = Record::database();
      QSqlQuery query(database);
      query.prepare(queryString);
      for (int i = 0; i < bindValues.size(); i++) {
        query.addBindValue(bindValues[i]);
      }
      if (!query.exec()) {
        qDebug() << "Query failed:" << queryString;
        return -1;
      }

      if (query.next()) {
        return query.value(0).toInt();
      }
      return -1;
    }

    Record(QObject *parent = 0);
    Record(const Record &other);
    Record &operator=(const Record &other);

    virtual const QString &tableName() const = 0;

    QVariant get(const QString &attributeName, bool dirty = true) const;
    void set(const QString &attributeName, const QVariant &value);
    void unset(const QString &attributeName);

    int id() const;
    bool isNew() const;
    bool wasNew() const;
    bool isModified() const;
    bool isValid();
    bool save();
    bool destroy();
    void reset();

    bool operator==(const Record &other);

  signals:
    void saved();
    void destroyed();

  protected:
    Record(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    virtual void beforeValidation();
    virtual bool validate();
    virtual void beforeCreate();
    virtual void beforeSave();
    virtual void afterSave();
    virtual void afterCreate();

  private:
    static QSqlDatabase s_database;
    static const QString s_emptyTableName;
    static bool executeFindQuery(QSqlQuery &query, const QString &tableName, const QString &select, const QString &conditions, const QList<QVariant> &bindValues, const QString &predicate);

    QMap<QString, QVariant> attributes;
    QMap<QString, QVariant> dirtyAttributes;
    bool newRecord, modified, wasNewRecord;
};

#endif
