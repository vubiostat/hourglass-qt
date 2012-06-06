#include <QtDebug>
#include <iostream>
#include <string>
#include "view.h"

View::View(QString name, bool useLayout, QObject *parent)
  : QObject(parent), useLayout(useLayout)
{
  if (useLayout) {
    _dictionary = new Dictionary("layout", this);
    _dictionary->setValue("title", "Hourglass");
    _contentDictionary = _dictionary->addIncludeDictionary("content", name);
    templateName = "layout.html";
  }
  else {
    _dictionary = _contentDictionary = new Dictionary("content", this);
    templateName = name;
  }
}

void View::setTitle(QString title)
{
  _dictionary->setValue("title", title);
}

Dictionary *View::dictionary()
{
  return _contentDictionary;
}

QString View::render()
{
  std::string out;

  bool result = ctemplate::ExpandTemplate(templateName.toStdString(),
      ctemplate::DO_NOT_STRIP, _dictionary->object(), &out);

  if (!result) {
    return QString("Couldn't render view!");
  }
  else {
    return QString::fromStdString(out);
  }
}
