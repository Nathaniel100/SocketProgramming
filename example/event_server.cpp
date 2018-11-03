
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>

#include <string.h>

void OnRead(struct bufferevent *bev, void *arg) {
  char buffer[1024] = {0};
  bufferevent_read(bev, buffer, sizeof(buffer));
  printf("read: %s\n", buffer);
  bufferevent_write(bev, buffer, strlen(buffer));
}

void OnWrite(struct bufferevent *bev, void *arg) {}

void OnEvent(struct bufferevent *bev, short what, void *arg) {
  if (what & BEV_EVENT_EOF) {
    printf("connection disconnected\n");
  } else if (what & BEV_EVENT_ERROR) {
    printf("error\n");
  }
  bufferevent_free(bev);
}

void OnNewConnection(struct evconnlistener *listener, evutil_socket_t fd,
                     struct sockaddr *addr, int socklen, void *arg) {
  struct event_base *base;
  struct bufferevent *bev;

  base = (struct event_base *)arg;
  bev = bufferevent_socket_new(base, fd,
                               BEV_OPT_DEFER_CALLBACKS | BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, OnRead, OnWrite, OnEvent, nullptr);
  bufferevent_enable(bev, EV_READ);
  bufferevent_enable(bev, EV_WRITE);
}

int main() {
  struct event_base *base;
  struct evconnlistener *listener;
  struct sockaddr_in sa;
  const int backlog = 128;

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(11789);
  sa.sin_addr.s_addr = INADDR_ANY;

  base = event_base_new();
  listener = evconnlistener_new_bind(
      base, OnNewConnection, base, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
      backlog, (struct sockaddr *)&sa, sizeof(sa));

  event_base_dispatch(base);
err:
  evconnlistener_free(listener);
  event_base_free(base);
  return 0;
}