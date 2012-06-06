#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QList>
#include <ctemplate/template.h>
#include "dictionary.h"

class View : public QObject
{
  Q_OBJECT

  public:
    View(QString name, bool useLayout = true, QObject *parent = 0);
    void setTitle(QString title);
    void addJavascript(QString url);
    Dictionary *dictionary();
    QString render();

  private:
    QString templateName;
    bool useLayout;
    Dictionary *_rootDictionary, *_layoutDictionary, *_contentDictionary;
};

#endif
