#include "thread.h"

void ServerThread::run()
{
  s = server_create((char *) "4567");
  if (server_start(s) == 1) {
    exec();
    server_stop(s);
    server_free(s);
  }
}
