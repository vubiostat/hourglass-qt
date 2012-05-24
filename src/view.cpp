#include <string>
#include "view.h"

View::View(QString name, QObject *parent)
  : QObject(parent)
{
  this->name = name;
}

void View::addVariable(const QString &name, const QString &str)
{
  rootFragment.addVariable(name.toStdString(), str.toStdString());
}

QString View::render()
{
  Teng::Teng_t teng(VIEW_PATH, Teng::Teng_t::Settings_t());
  std::string out;
  Teng::StringWriter_t writer(out);
  Teng::Error_t err;

  int result = teng.generatePage(
      name.toStdString(),
      std::string(),
      std::string(),
      std::string(),
      std::string(),
      "text/html",
      "utf-8",
      rootFragment,
      writer,
      err);

  if (result == 0) {
    return QString::fromStdString(out);
  }
  else {
    return QString("zomg errorz");
  }
}
