#ifndef SETTING_H
#define SETTING_H

#include <QTime>
#include <QRegExp>
#include "model.h"

class Setting : public Model
{
  Q_OBJECT

  public:
    static QList<Setting> find();
    static QList<Setting> find(QString conditions);
    static QList<Setting> find(QString conditions, const QList<QVariant> &bindValues);
    static Setting findByKey(QString key);

    static QString getValue(QString key, QString defaultValue);
    static QTime getDayStart(QString defaultValue);
    static QTime getDayEnd(QString defaultValue);
    static bool setValue(QString key, QString value);

    Setting(QObject *parent = 0) : Model(parent) {}
    Setting(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0)
      : Model(attributes, newRecord, parent) {}

    QString key() const;
    void setKey(const QString &key);
    QString value() const;
    void setValue(const QString &value);

    bool save();

  private:
    static const QRegExp timePattern;

    bool validate();
};

#endif
