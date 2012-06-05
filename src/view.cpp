#include <QtDebug>
#include <iostream>
#include <string>
#include "view.h"

View::View(QString name, bool useLayout, QObject *parent)
  : QObject(parent), name(name), useLayout(useLayout)
{
}

void View::setTitle(QString title)
{
  this->title = title;
}

void View::addVariable(const QString &name, QVariant value)
{
  variables[name] = value;
}

QString View::render()
{
  Teng::Teng_t teng(VIEW_PATH, Teng::Teng_t::Settings_t());
  std::string contentOut, layoutOut;
  Teng::StringWriter_t contentWriter(contentOut), layoutWriter(layoutOut);
  Teng::Error_t contentErr, layoutErr;
  Teng::Fragment_t contentFragment, layoutFragment;

  setupFragment(contentFragment, variables);
  int result = teng.generatePage(
      name.toStdString(),
      std::string(),
      std::string(),
      std::string(),
      std::string(),
      "text/html",
      "utf-8",
      contentFragment,
      contentWriter,
      contentErr);

  if (result != 0) {
    contentErr.dump(std::cout);
    return QString("Content error!");
  }

  if (useLayout) {
    if (!title.isEmpty()) {
      layoutFragment.addVariable("title", title.toStdString());
    }
    else {
      layoutFragment.addVariable("title", "Hourglass");
    }
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

void View::setupFragment(Teng::Fragment_t &fragment, QVariantMap &vars)
{
  QMapIterator<QString, QVariant> i(vars);
  while (i.hasNext()) {
    i.next();
    switch (i.value().type()) {
      case QVariant::String:
        fragment.addVariable(i.key().toStdString(), i.value().toString().toStdString());
        break;
      case QVariant::Int:
        fragment.addVariable(i.key().toStdString(), (Teng::IntType_t) i.value().toInt());
        break;
      case QVariant::Map:
        {
          Teng::Fragment_t &subFragment = fragment.addFragment(i.key().toStdString());
          QVariantMap subVars = i.value().toMap();
          setupFragment(subFragment, subVars);
        }
        break;
      case QVariant::List:
        {
          Teng::FragmentList_t &subFragmentList =
            fragment.addFragmentList(i.key().toStdString());
          QVariantList subVars = i.value().toList();
          setupFragmentList(subFragmentList, subVars);
        }
        break;
      default:
        qDebug() << "Invalid QVariant type for variable" << i.key() << ":" << i.value().type();
        break;
    }
  }
}

void View::setupFragmentList(Teng::FragmentList_t &fragmentList, QVariantList &vars)
{
  for (int i = 0; i < vars.count(); i++) {
    if (vars[i].type() == QVariant::Map) {
      Teng::Fragment_t &subFragment = fragmentList.addFragment();
      QVariantMap subVars = vars[i].toMap();
      setupFragment(subFragment, subVars);
    }
    else {
      qDebug() << "Invalid QVariant type for list" << ":" << vars[i].type();
    }
  }
}
