#ifndef SERVER_H
#define SERVER_H

#include "mongoose.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct _server {
  struct mg_context *ctx;
  char *port;
} server;

server *server_create(char *port);
int server_start(server *s);
void server_stop(server *s);
void server_free(server *s);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
