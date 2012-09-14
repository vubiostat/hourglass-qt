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

    template <class T>
    static QList<T> find(QString tableName)
    {
      return find<T>(tableName, QString());
    }

    template <class T>
    static QList<T> find(QString tableName, QString conditions)
    {
      QList<QVariant> emptyBindValues;
      return find<T>(tableName, conditions, emptyBindValues);
    }

    template <class T>
    static QList<T> find(QString tableName, QString conditions, QString predicate)
    {
      QList<QVariant> emptyBindValues;
      return find<T>(tableName, conditions, emptyBindValues, predicate);
    }

    template <class T>
    static T findById(QString tableName, int id)
    {
      QList<QVariant> bindValues;
      bindValues << QVariant(id);
      QList<T> elements = find<T>(tableName, "WHERE id = ?", bindValues);
      if (elements.size() != 1) {
        return T();
      }
      return elements[0];
    }

    template <class T>
    static QList<T> find(QString tableName, QString conditions, const QList<QVariant> &bindValues)
    {
      return find<T>(tableName, conditions, bindValues, QString());
    }

    template <class T>
    static QList<T> find(QString tableName, QString conditions, const QList<QVariant> &bindValues, QString predicate)
    {
      QStringList queryStrings;
      queryStrings << "SELECT * FROM";
      queryStrings << tableName;
      if (!conditions.isEmpty()) {
        queryStrings << conditions;
      }
      if (!predicate.isEmpty()) {
        queryStrings << predicate;
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
        return QList<T>();
      }

      QList<T> result;
      while (query.next()) {
        QSqlRecord record = query.record();
        QMap<QString, QVariant> attributes;
        for (int i = 0; i < record.count(); i++) {
          attributes.insert(record.fieldName(i), query.value(i));
        }

        T elt(attributes, false);
        result << elt;
      }
      return result;
    }

    static int count(QString tableName);
    static int count(QString tableName, QString conditions);
    static int count(QString tableName, QString conditions, const QList<QVariant> &bindValues);

    Record(QObject *parent = 0);
    Record(const Record &other);
    Record &operator=(const Record &other);

    QVariant get(const QString &attributeName) const;
    void set(const QString &attributeName, QVariant value);
    void unset(const QString &attributeName);

    int id() const;
    bool isNew() const;
    bool isModified() const;
    bool isValid();
    bool save(QString tableName);
    bool destroy(QString tableName);

    bool operator==(const Record &other);

  protected:
    Record(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);
    static QSqlDatabase database();

    virtual void beforeValidation();
    virtual bool validate();
    virtual void beforeCreate();
    virtual void beforeSave();
    virtual void afterSave();
    virtual void afterCreate();

  private:
    QMap<QString, QVariant> attributes;
    bool newRecord, modified;
};

#endif
