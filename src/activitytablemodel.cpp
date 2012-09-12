#include "activitytablemodel.h"
#include <QSqlRecord>
#include <QStringList>
#include <QIcon>
#include <QtDebug>

const QString ActivityTableModel::s_queryString = QString(
    "SELECT activities.id, activities.name, "
    "projects.name as project_name, activities.started_at, "
    "activities.ended_at, activities.untimed, activities.duration, "
    "activities.day FROM activities LEFT JOIN projects "
    "ON activities.project_id = projects.id ORDER BY activities.id");

const QString ActivityTableModel::s_dayQueryString = QString(
    "SELECT activities.id, activities.name, "
    "projects.name as project_name, activities.started_at, "
    "activities.ended_at, activities.untimed, activities.duration, "
    "activities.day FROM activities LEFT JOIN projects "
    "ON activities.project_id = projects.id "
    "WHERE date(activities.started_at) = date('%1') OR "
    "(activities.untimed = 1 AND date(activities.day) = date('%1')) "
    "ORDER BY activities.id");

const QString ActivityTableModel::s_timeSeparator = QString("-");

ActivityTableModel::ActivityTableModel(QObject *parent, QSqlDatabase database)
  : QAbstractTableModel(parent), m_database(database)
{
  m_query = QSqlQuery(s_queryString, m_database);
  getActivities();
}

ActivityTableModel::ActivityTableModel(QDate date, QObject *parent, QSqlDatabase database)
  : QAbstractTableModel(parent), m_database(database)
{
  m_query = QSqlQuery(s_dayQueryString.arg(date.toString(Qt::ISODate)), m_database);
  getActivities();
}

Qt::ItemFlags ActivityTableModel::flags(const QModelIndex &index) const
{
  if (index.column() < 6) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
  else {
    return Qt::ItemIsEnabled;
  }
}

int ActivityTableModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_activities.count();
}

int ActivityTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 9;
}

QVariant ActivityTableModel::data(const QModelIndex &index, int role) const
{
  const QVariantHash &hash = m_activities.at(index.row());
  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case 0:
          if (hash.value("untimed").toInt() == 1) {
            return QVariant();
          }
          return formatDateTime(hash.value("started_at"));

        case 1:
          if (hash.value("untimed").toInt() == 1) {
            return QVariant();
          }
          return s_timeSeparator;

        case 2:
          if (hash.value("untimed").toInt() == 1) {
            return QVariant();
          }
          return formatDateTime(hash.value("ended_at"));

        case 3:
          return hash.value("name");

        case 4:
          return hash.value("project_name");

        case 5:
          /* Tags */
          return QVariant();

        case 6:
          return durationInWords(duration(hash));
      }
      break;

    case Qt::DecorationRole:
      switch (index.column()) {
        case 7:
          return QIcon::fromTheme("accessories-text-editor");
        case 8:
          return QIcon::fromTheme("user-trash");
      }

      break;

    case Qt::TextAlignmentRole:
      if (index.column() >= 3 && index.column() <= 5) {
        return (int) (Qt::AlignVCenter | Qt::AlignLeft);
      }
      else if (index.column() == 6) {
        return (int) (Qt::AlignVCenter | Qt::AlignRight);
      }
      else {
        return Qt::AlignCenter;
      }

    case Qt::ForegroundRole:
      if (index.column() == 4) {
        return Qt::gray;
      }
      break;

    default:
      return QVariant();
  }
  return QVariant();
}

void ActivityTableModel::getActivities()
{
  while (m_query.next()) {
    QVariantHash hash;
    QSqlRecord record = m_query.record();
    hash.insert("id",           record.value("id"));
    hash.insert("name",         record.value("name"));
    hash.insert("project_name", record.value("project_name"));
    hash.insert("started_at",   record.value("started_at"));
    hash.insert("ended_at",     record.value("ended_at"));
    hash.insert("untimed",      record.value("untimed"));
    hash.insert("duration",     record.value("duration"));
    hash.insert("day",          record.value("day"));
    m_activities.append(hash);
  }
}

QString ActivityTableModel::formatDateTime(QVariant value) const
{
  return value.toDateTime().toString("hh:mm");
}

int ActivityTableModel::duration(const QVariantHash &hash) const
{
  if (hash.value("untimed").toInt() == 1) {
    return hash.value("duration").toInt();
  }
  else {
    QDateTime startedAt = hash.value("started_at").toDateTime();
    if (startedAt.isValid()) {
      QDateTime endedAt = hash.value("ended_at").toDateTime();
      if (endedAt.isValid()) {
        return startedAt.secsTo(endedAt);
      }
      else {
        return startedAt.secsTo(QDateTime::currentDateTime());
      }
    }
    else {
      return -1;
    }
  }
}

QString ActivityTableModel::durationInWords(int duration) const
{
  if (duration >= 0) {
    int totalMinutes = duration / 60;

    if (totalMinutes == 0) {
      return QString("0min");
    }
    else {
      int minutes = totalMinutes % 60;
      int totalHours = totalMinutes / 60;
      int hours = totalHours % 24;
      int days = totalHours / 24;
      //qDebug() << "minutes:" << minutes << "; hours:" << hours << "; days:" << days;

      QStringList strings;
      if (days > 0) {
        strings << QString("%1d").arg(days);
      }
      if (hours > 0) {
        strings << QString("%1h").arg(hours);
      }
      if (minutes > 0) {
        strings << QString("%1min").arg(minutes);
      }
      return strings.join(" ");
    }
  }
  else {
    return QString();
  }
}
