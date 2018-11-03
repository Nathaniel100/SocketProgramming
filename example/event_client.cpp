
#include <event2/bufferevent.h>
#include <event2/event.h>

#include <stdlib.h>
#include <string.h>

#include <socket/socket.h>

void Usage() { printf("EventClient SERVER_IP SERVER_PORT\n"); }

static void OnRead(struct bufferevent *bev, void *arg) {
  char buffer[100] = {0};
  int nread = bufferevent_read(bev, buffer, sizeof(buffer));
  if (nread == 5) {
    printf("read: %s\n", buffer);
  } else if (nread == 0) {
    printf("connection disconnected\n");
  } else {
    fprintf(stderr, "read failed\n");
  }
  bufferevent_free(bev);
}

static void OnWrite(struct bufferevent *bev, void *arg) {
  printf("write success\n");
}

static void OnEvent(struct bufferevent *bev, short what, void *arg) {
  if (what == BEV_EVENT_CONNECTED) {
    printf("connect success\n");
    bufferevent_write(bev, "Hello", 5);
  } else if (what == BEV_EVENT_EOF) {
    printf("Peer disconnected\n");
  } else if (what == BEV_EVENT_ERROR) {
    printf("Connection error\n");
  }
}

int main(int argc, char **argv) {
  struct event_base *base;
  struct bufferevent *bev;
  const char *server_address;
  int port = 0;
  struct sockaddr_in addr;

  if (argc != 3) {
    Usage();
    return -1;
  }
  server_address = argv[1];
  port = strtol(argv[2], nullptr, 0);
  if (!sock::SetAddress(&addr, server_address, port)) {
    Usage();
    return -1;
  }

  base = event_base_new();

  bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, OnRead, OnWrite, OnEvent, base);
  bufferevent_enable(bev, EV_READ);
  bufferevent_enable(bev, EV_WRITE);
  if (bufferevent_socket_connect(bev, (struct sockaddr *)&addr, sizeof(addr)) !=
      0) {
    fprintf(stderr, "connect failed\n");
    goto err;
  }

  event_base_dispatch(base);

err:
  event_base_free(base);
  return 0;
}