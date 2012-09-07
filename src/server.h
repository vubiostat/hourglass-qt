#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QSqlDatabase>
#include <QList>
#include <QPair>
#include <QDir>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

class Server : public QObject
{
  Q_OBJECT

  public:
    Server(QString root, quint16 port, QObject *parent = 0);
    void start();

  signals:
    void started();
    void stopped();

  public slots:
    void stop();
    void handle(QHttpRequest *req, QHttpResponse *resp);

  private:
    QHttpServer *http;
    QDir root;
    quint16 port;
};

#endif
