#include <stdlib.h>
#include "server.h"

server *
server_create(port)
  char *port;
{
  server *s = (server *) malloc(sizeof(server));
  s->ctx = NULL;
  s->port = port;
  return s;
}

static void *
server_callback(event, conn, request_info)
  enum mg_event event;
  struct mg_connection *conn;
  const struct mg_request_info *request_info;
{
  if (event == MG_NEW_REQUEST) {
    // Echo requested URI back to the client
    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", request_info->uri);
    return "";  // Mark as processed
  } else {
    return NULL;
  }
}

int
server_start(s)
  server *s;
{
  const char *options[] = {"listening_ports", s->port, NULL};
  s->ctx = mg_start(&server_callback, NULL, options);
  if (s->ctx == NULL) {
    return 0;
  }
  return 1;
}

void
server_stop(s)
  server *s;
{
  if (s->ctx != NULL) {
    mg_stop(s->ctx);
    s->ctx = NULL;
  }
}

void
server_free(s)
  server *s;
{
  if (s->ctx != NULL) {
    server_stop(s);
  }
  free(s);
}
