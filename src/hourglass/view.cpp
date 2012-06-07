#include <QtDebug>
#include <iostream>
#include <string>
#include "view.h"

View::View(QString name, bool useLayout, QObject *parent)
  : QObject(parent), useLayout(useLayout)
{
  if (useLayout) {
    _rootDictionary = _layoutDictionary = new Dictionary("layout", this);
    _layoutDictionary->setValue("title", "Hourglass");
    _contentDictionary = _layoutDictionary->addIncludeDictionary("content", name);
    templateName = "layout.html";
  }
  else {
    _rootDictionary = _contentDictionary = new Dictionary("content", this);
    _layoutDictionary = NULL;
    templateName = name;
  }
}

void View::setTitle(QString title)
{
  if (_layoutDictionary != NULL) {
    _layoutDictionary->setValue("title", title);
  }
}

void View::addJavascript(QString url)
{
  if (_layoutDictionary != NULL) {
    Dictionary *d = _layoutDictionary->addSectionDictionary("javascript");
    d->setValue("url", url);
  }
}

Dictionary *View::dictionary()
{
  return _contentDictionary;
}

QString View::render()
{
  std::string out;

  bool result = ctemplate::ExpandTemplate(templateName.toStdString(),
      ctemplate::DO_NOT_STRIP, _rootDictionary->object(), &out);

  if (!result) {
    return QString("Couldn't render view!");
  }
  else {
    return QString::fromStdString(out);
  }
}
