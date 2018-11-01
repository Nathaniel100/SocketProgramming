//
// Created by 吴凡 on 2017/10/20.
//

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "socket/log.h"
#include "socket/socket.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 11789
#define BUFFER_SIZE 2048

int main() {
  sock::Socket client_socket;
  if (!sock::CreateClientSocket(SERVER_ADDR, SERVER_PORT, &client_socket)) {
    return -1;
  }

  fd_set rdset;
  FD_ZERO(&rdset);
  char buf[BUFFER_SIZE] = {0};
  while(true) {
    FD_SET(0, &rdset);
    FD_SET(client_socket.fd(), &rdset);
    if (select(client_socket.fd() + 1, &rdset, nullptr, nullptr, nullptr) < 0) {
      LOG_E("select failed: %s", strerror(errno));
      return -1;
    }

    if (FD_ISSET(0, &rdset)) {
      memset(buf, 0, sizeof(buf));
      if (std::cin.getline(buf, sizeof(buf))) {
        if (!client_socket.Send(buf, strlen(buf), nullptr)) {
          return -1;
        }
      } else {
        break;
      }
    }
    if (FD_ISSET(client_socket.fd(), &rdset)) {
      memset(buf, 0, sizeof(buf));
      if (!client_socket.Recv(sizeof(buf), buf, nullptr)) {
        return -1;
      }
      LOG_D("Read: %s", buf);
    }
  }

  client_socket.Close();
  return 0;
}
