#include "setting.h"

QList<Setting> Setting::find()
{
  return Model::find<Setting>("settings");
}

QList<Setting> Setting::find(QString conditions)
{
  return Model::find<Setting>("settings", conditions);
}

QList<Setting> Setting::find(QString conditions, const QList<QVariant> &bindValues)
{
  return Model::find<Setting>("settings", conditions, bindValues);
}

Setting Setting::findByKey(QString key)
{
  QList<QVariant> bindValues;
  bindValues << key;
  QList<Setting> settings = find("WHERE key = ?", bindValues);
  if (settings.size() > 0) {
    return settings[0];
  }
  return Setting();
}

QString Setting::getValue(QString key, QString defaultValue)
{
  Setting s = findByKey(key);
  if (!s.isNew()) {
    return s.value();
  }
  return defaultValue;
}

QTime Setting::getDayStart(QString defaultValue)
{
  return QTime::fromString(getValue("day_start", defaultValue), "hh:mm");
}

QTime Setting::getDayEnd(QString defaultValue)
{
  return QTime::fromString(getValue("day_end", defaultValue), "hh:mm");
}

QString Setting::key() const
{
  return get("key").toString();
}

void Setting::setKey(const QString &key)
{
  set("key", key);
}

QString Setting::value() const
{
  return get("value").toString();
}

void Setting::setValue(const QString &value)
{
  set("value", value);
}

bool Setting::save()
{
  return Model::save("settings");
}
