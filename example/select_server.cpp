//
// Created by 吴凡 on 2018/11/1.
//

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>

#include "socket/log.h"
#include "socket/socket.h"

#define SERVER_PORT 11789
#define BUFFER_SIZE 2048

int main() {
  sock::Socket server_socket;
  if (!sock::CreateServerSocket("127.0.0.1", SERVER_PORT, &server_socket)) {
    return -1;
  }

  int maxfd = 0;
  sock::Socket clients[FD_SETSIZE];
  int maxi = -1;
  int i;
  int nready;
  fd_set rdset, allset;
  FD_ZERO(&allset);
  FD_SET(server_socket.fd(), &allset);
  maxfd = server_socket.fd();

  while (1) {
    rdset = allset;
    nready = select(maxfd + 1, &rdset, nullptr, nullptr, nullptr);
    if ( nready < 0 ) {
      LOG_E("select failed: %s", strerror(errno));
      break;
    }
    if (FD_ISSET(server_socket.fd(), &rdset)) {
      sock::Socket client_socket;
      if (!server_socket.Accept(&client_socket)) {
        return -1;
      }
      for (i = 0; i < FD_SETSIZE; i++) {
        if (clients[i].fd() == -1) {
          clients[i] = client_socket;
          break;
        }
      }
      if (client_socket.fd() > maxfd) {
        maxfd = client_socket.fd();
      }
      if (i > maxi) {
        maxi = i;
      }
      FD_SET(client_socket.fd(), &allset);
      if (--nready <= 0) continue;
    }

    for (i = 0; i <= maxi; i++) {
      if (clients[i].fd() == -1) {
        continue;
      }
      if (FD_ISSET(clients[i].fd(), &rdset)) {
        char buf[BUFFER_SIZE] = {0};
        size_t nread = 0;
        if (!clients[i].Recv(sizeof(buf), buf, &nread)) {
          clients[i].Close();
          FD_CLR(clients[i].fd(), &allset);
          clients[i] = sock::Socket();
          continue;
        }
        if (!clients[i].Send(buf, nread, nullptr)) {
          clients[i].Close();
          FD_CLR(clients[i].fd(), &allset);
          clients[i] = sock::Socket();
          continue;
        }
        if (--nready <= 0) {
          break;
        }
      }
    }
  }
  server_socket.Close();

  return 0;
}
