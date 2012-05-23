#include <QApplication>
#include <iostream>
#include "launcher.h"
#include "thread.h"
using namespace std;

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  ServerThread t;
  t.start();

  Launcher launcher;
  launcher.show();

  int result = app.exec();
  t.quit();
  t.wait();

  return result;
}
