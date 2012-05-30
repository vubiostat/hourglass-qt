#include <QtDebug>
#include <iostream>
#include <string>
#include "view.h"

View::View(QString name, bool useLayout, QObject *parent)
  : QObject(parent), name(name), useLayout(useLayout)
{
}

QString View::render(VariableMap &variables)
{
  Teng::Teng_t teng(VIEW_PATH, Teng::Teng_t::Settings_t());
  std::string contentOut, layoutOut;
  Teng::StringWriter_t contentWriter(contentOut), layoutWriter(layoutOut);
  Teng::Error_t contentErr, layoutErr;
  Teng::Fragment_t layoutFragment;

  Teng::Fragment_t *contentFragment = variables.fragment();
  int result = teng.generatePage(
      name.toStdString(),
      std::string(),
      std::string(),
      std::string(),
      std::string(),
      "text/html",
      "utf-8",
      *contentFragment,
      contentWriter,
      contentErr);
  delete contentFragment;

  if (result != 0) {
    contentErr.dump(std::cout);
    return QString("Content error!");
  }

  if (useLayout) {
    layoutFragment.addVariable("content", contentOut);
    result = teng.generatePage(
        "layout.html",
        std::string(),
        std::string(),
        std::string(),
        std::string(),
        "text/html",
        "utf-8",
        layoutFragment,
        layoutWriter,
        layoutErr);

    if (result != 0) {
      layoutErr.dump(std::cout);
      return QString("Layout error!");
    }

    return QString::fromStdString(layoutOut);
  }
  else {
    return QString::fromStdString(contentOut);
  }
}
