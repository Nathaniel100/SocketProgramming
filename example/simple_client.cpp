//
// Created by 吴凡 on 2017/10/20.
//

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include "socket/socket.h"
#include "socket/log.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 11789
#define BUFFER_SIZE 2048

int main() {
  sock::Socket client_socket;
  if (!sock::CreateClientSocket(SERVER_ADDR, SERVER_PORT, &client_socket)) {
    return -1;
  }

  char buf[BUFFER_SIZE] = {0};
  while (std::cin.getline(buf, sizeof(buf))) {
    if (!client_socket.Send(buf, strlen(buf), nullptr)) {
      return -1;
    }
    memset(buf, 0, sizeof(buf));
    if (!client_socket.Recv(sizeof(buf), buf, nullptr)) {
      return -1;
    }
    LOG_D("Read: %s", buf);
    memset(buf, 0, sizeof(buf));
  }

  client_socket.Close();
  return 0;
}
