#include <signal.h>
#include "hourglass.h"

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

int main(int argc, char **argv)
{
  Hourglass app(argc, argv);
  setup_unix_signal_handlers();
  return app.exec();
}
