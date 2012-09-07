#include "hourglass.h"

#ifndef Q_OS_WIN32
#include <signal.h>
static int setup_unix_signal_handlers()
{
  struct sigaction interrupt;

  interrupt.sa_handler = Hourglass::intSignalHandler;
  sigemptyset(&interrupt.sa_mask);
  interrupt.sa_flags = 0;

  if (sigaction(SIGINT, &interrupt, 0) > 0)
    return 1;

  return 0;
}
#endif

int main(int argc, char **argv)
{
  Hourglass app(argc, argv);
#ifndef Q_OS_WIN32
  setup_unix_signal_handlers();
#endif
  return app.exec();
}
