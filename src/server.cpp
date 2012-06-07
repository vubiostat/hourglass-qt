#include <QtDebug>
#include <QByteArray>
#include <QUrl>
#include <QDir>
#include <QStringList>
#include <QVariant>
#include <QMap>
#include <QList>
#include "server.h"
#include "project.h"
#include "tag.h"
#include "view.h"

#include <iostream>
using namespace std;

const QRegExp Server::editActivityPath = QRegExp("^/activities/(\\d+)/edit$");
const QRegExp Server::activityPath = QRegExp("^/activities/(\\d+)$");
const QRegExp Server::deleteActivityPath = QRegExp("^/activities/(\\d+)/delete$");
const QRegExp Server::restartActivityPath = QRegExp("^/activities/(\\d+)/restart$");

Server::Server(QString root, quint16 port, QObject *parent)
  : QObject(parent), port(port)
{
  http = new QHttpServer(this);
  connect(http, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
      this, SLOT(route(QHttpRequest*, QHttpResponse*)));
  this->root = QDir(root);
  qDebug() << "Root directory:" << this->root.dirName();
}

void Server::route(QHttpRequest *request, QHttpResponse *response)
{
  QString path = request->path();
  QString method = request->method();

  QString result;
  bool isJSON = false;
  if (method == "GET") {
    if (path == "/") {
      result = index();
    }
    else if (path == "/activities/new") {
      result = newActivity();
    }
    else if (path.contains(editActivityPath)) {
    }
    else if (path.contains(deleteActivityPath)) {
    }
    else if (path == "/activities") {
      result = partialActivityNames();
      isJSON = true;
    }
    else if (path == "/activities/current/stop") {
      result = stopCurrentActivities();
      isJSON = true;
    }
    else if (path == "/tags") {
      result = partialTagNames();
      isJSON = true;
    }
    else if (path == "/projects") {
      result = partialProjectNames();
      isJSON = true;
    }
    else if (path == "/totals") {
      result = partialTotals();
      isJSON = true;
    }
  }
  else if (method == "POST") {
    /*
    const char *tmp = mg_get_header(conn, "Content-Length");
    if (tmp != NULL) {
      QString contentLengthHeader = QString(tmp);
      bool ok;
      int contentLength = contentLengthHeader.toInt(&ok);
      if (ok) {
        //qDebug() << "POST Content-Length:" << contentLength;
        char *buffer = new char[contentLength + 1];
        if (mg_read(conn, buffer, contentLength) == contentLength) {
          // Valid POST
          buffer[contentLength] = 0;
          //qDebug() << "POST Content:" << buffer;
          QList<QPair<QString, QString> > params = decodePost(buffer);

          if (path == "/activities") {
            result = createActivity(params);
            isJSON = true;
          }
          else if (path.contains(activityPath)) {
          }
          else if (path.contains(restartActivityPath)) {
          }
        }
        delete[] buffer;
      }
    }
    */
  }

  if (result.isNull()) {
    // Serve a file
    QFile file(root.absoluteFilePath(path));
    if (file.exists()) {
      qDebug() << "Serving file:" << file.fileName();
      QFileInfo info(file);
      QString contentType("text/plain");
      if (info.suffix() == "css") {
        contentType = "text/css";
      }
      else if (info.suffix() == "js") {
        contentType = "text/javascript";
      }
      else if (info.suffix() == "png") {
        contentType = "image/png";
      }
      else if (info.suffix() == "gif") {
        contentType = "image/gif";
      }
      response->setHeader("Content-Type", contentType);
      response->setHeader("Content-Length", QString("%1").arg(file.size()));
      response->writeHead(200);
      response->write(file.readAll());
    }
    else {
      qDebug() << "File doesn't exist:" << file.fileName();
      response->writeHead(404);
    }
  }
  else {
    response->setHeader("Content-Type",
        isJSON ? "application/json" : "text/html");
    response->setHeader("Content-Length", QString("%1").arg(result.size()));
    response->writeHead(200);
    response->write(result);
  }
  response->end();
}

QList<QPair<QString, QString> > Server::decodePost(const char *data)
{
  QList<QPair<QString, QString> > result;

  QStringList parts = QString(data).split('&');
  for (int i = 0; i < parts.size(); i++) {
    QString variable = parts[i];
    QStringList variableParts = variable.split('=');
    if (variableParts.size() == 2) {
      QString &rawKey = variableParts[0].replace("+", " ");
      QString &rawValue = variableParts[1].replace("+", " ");
      QByteArray rawKeyBA = rawKey.toUtf8();
      QByteArray rawValueBA = rawValue.toUtf8();
      QPair<QString, QString> pair(
          QUrl::fromPercentEncoding(rawKeyBA),
          QUrl::fromPercentEncoding(rawValueBA));
      //qDebug() << pair;
      result << pair;
    }
  }
  return result;
}

void Server::start()
{
  http->listen(port);
  emit started();
}

void Server::stop()
{
  emit stopped();
}

void Server::includeActivities(Dictionary *dictionary, QList<Activity> &activities)
{
  Dictionary *d = dictionary->addIncludeDictionary("activities", "_activities.html");
  d->addActivitySection(activities);
}

void Server::includeNames(Dictionary *dictionary, QList<QString> &names)
{
  Dictionary *d = dictionary->addIncludeDictionary("names", "_names.js");

  Dictionary *d2;
  QListIterator<QString> i(names);
  while (i.hasNext()) {
    d2 = d->addSectionDictionary("name");
    d2->setValue("value", i.next());
    d2->setValue("comma", i.hasNext() ? "," : "");
  }
}

void Server::includeCurrent(Dictionary *dictionary)
{
  Dictionary *d = dictionary->addIncludeDictionary("current", "_current.html");
  QList<Activity> activities = Activity::findCurrent();
  if (activities.count() > 0) {
    d->addActivitySection(activities);
  }
  else {
    d->showSection("noCurrentActivity");
  }
}

void Server::includeToday(Dictionary *dictionary)
{
  Dictionary *d = dictionary->addIncludeDictionary("today", "_today.html");
  QList<Activity> activities = Activity::findToday();
  includeActivities(d, activities);
}

void Server::includeWeek(Dictionary *dictionary)
{
  Dictionary *d = dictionary->addIncludeDictionary("week", "_week.html");

  // Start day at Sunday
  QDate day;
  QDate today = QDate::currentDate();
  int dayOfWeek = today.dayOfWeek();
  if (dayOfWeek != Qt::Sunday) {
    day = today.addDays(-dayOfWeek);
  }
  else {
    day = today;
  }

  Dictionary *d2;
  for (int i = 0; i < 7; i++) {
    d2 = d->addSectionDictionary("day");
    QList<Activity> activities = Activity::findDay(day);

    dayOfWeek = day.dayOfWeek();
    d2->setValue("dayName", QDate::longDayName(dayOfWeek));
    d2->setValue("dayNumber", i);
    includeActivities(d2, activities);

    day = day.addDays(1);
  }
}

void Server::includeTotals(Dictionary *dictionary, bool addIncludeDictionary)
{
  Dictionary *d;
  if (addIncludeDictionary) {
    d = dictionary->addIncludeDictionary("totals", "_totals.html");
  }
  else {
    d = dictionary;
  }

  QList<Activity> activities = Activity::findToday();
  QMap<QString, int> projectTotals = Activity::projectTotals(activities);

  Dictionary *d2;
  QMapIterator<QString, int> i(projectTotals);
  while (i.hasNext()) {
    i.next();
    d2 = d->addSectionDictionary("total");
    d2->setValue("projectName", i.key().isEmpty() ? "Unsorted" : i.key());
    d2->setValue("duration", i.value());
    d2->setValue("durationInWords",
        QString("%1").arg(((double) i.value()) / 3600.0, 4, 'f', 2, '0'));
    d2->setValue("comma", i.hasNext() ? "," : "");
  }
}

QString Server::partialTotals()
{
  View view("_totals.html", false);
  Dictionary *dictionary = view.dictionary();
  includeTotals(dictionary, false);
  return view.render();
}

QString Server::partialUpdates()
{
  View view("_updates.js", false);
  Dictionary *dictionary = view.dictionary();
  includeCurrent(dictionary);
  includeToday(dictionary);
  includeWeek(dictionary);
  includeTotals(dictionary);
  return view.render();
}

QString Server::partialNames(QList<QString> &names)
{
  View view("_names.js", false);
  Dictionary *d = view.dictionary();

  Dictionary *d2;
  QListIterator<QString> i(names);
  while (i.hasNext()) {
    QString name = i.next();
    d2 = d->addSectionDictionary("name");
    d2->setValue("value", name);
    d2->setValue("comma", i.hasNext() ? "," : "");
  }

  return view.render();
}

QString Server::partialActivityNames()
{
  QList<QString> distinctNames = Activity::distinctNames();
  return partialNames(distinctNames);
}

QString Server::partialProjectNames()
{
  QList<QString> distinctNames = Project::distinctNames();
  return partialNames(distinctNames);
}

QString Server::partialTagNames()
{
  QList<QString> distinctNames = Tag::distinctNames();
  return partialNames(distinctNames);
}

// GET /
QString Server::index()
{
  View view("index.html");
  view.addJavascript("/js/index.js");
  Dictionary *dictionary = view.dictionary();
  includeCurrent(dictionary);
  includeToday(dictionary);
  includeWeek(dictionary);
  includeTotals(dictionary);
  return view.render();
}

// POST /activities
QString Server::createActivity(const QList<QPair<QString, QString> > &params)
{
  Activity activity;
  activity.setStartedAt(QDateTime::currentDateTime());
  activity.setFromParams(params);
  if (activity.save()) {
    return partialUpdates();
  }
  return QString("{\"errors\": \"There were errors!\"}");
}

// GET /activities/current/stop
QString Server::stopCurrentActivities()
{
  Activity::stopCurrent();
  return partialUpdates();
}

// GET /activities/new
QString Server::newActivity()
{
  View view("popup.html");
  view.setTitle("Add earlier activity");
  Dictionary *dictionary = view.dictionary();

  Activity newActivity = Activity();
  dictionary->setValue("submitUrl", "/activities");
  dictionary->addActivitySection(newActivity);
  return view.render();
}
