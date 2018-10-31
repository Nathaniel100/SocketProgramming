//
// Created by 吴凡 on 2017/10/20.
//

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>

#include "socket/socket.h"
#include "socket/log.h"

#define SERVER_PORT 11789
#define BUFFER_SIZE 2048

int main() {
  sock::Socket server_socket;
  if (!sock::CreateServerSocket("127.0.0.1", SERVER_PORT, &server_socket)) {
    return -1;
  }

  while(1) {
    sock::Socket client_socket;
    if (!server_socket.Accept(&client_socket)) {
      return -1;
    }

    char buf[BUFFER_SIZE] = {0};
    while(1) {
      if (!client_socket.Recv(sizeof(buf), buf, nullptr)) {
        client_socket.Close();
        break;
      }
      if (!client_socket.Send(buf, strlen(buf), nullptr)) {
        client_socket.Close();
        break;
      }
      memset(buf, 0, sizeof(buf));
    }
  }
  server_socket.Close();

  return 0;
}