//
// Created by 吴凡 on 2017/10/20.
//

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>

#define SERVER_PORT 11789

int main() {
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(listenSocket == -1) {
    std::cerr << "socket failed\n";
    return 1;
  }
  sockaddr_in bindAddr;
  memset(&bindAddr, 0, sizeof(bindAddr));
  bindAddr.sin_family = AF_INET;
  bindAddr.sin_port = htons(SERVER_PORT);
  bindAddr.sin_addr.s_addr = INADDR_ANY;

  if(bind(listenSocket, (sockaddr *)&bindAddr, sizeof(bindAddr)) == -1) {
    std::cerr << "bind failed\n";
    return 1;
  }

  if(listen(listenSocket, 20) == -1) {
    std::cerr << "listen failed\n";
    return 1;
  }


  char buf[100] = {0};
  while(1) {
    int acceptSocket = accept(listenSocket, nullptr, nullptr);
    if(acceptSocket <= 0) {
      std::cerr << "accept failed\n";
      return 1;
    }

    while(1) {
      int nread = read(acceptSocket, buf, 100);
      if (nread == 0) {
        std::cout << "socket is closed\n";
        close(acceptSocket);
        break;
      } else if (nread > 0) {
        std::cout << buf << "\n";
      } else {
        std::cerr << "read failed\n";
        close(acceptSocket);
        break;
      }

      if (write(acceptSocket, buf, strlen(buf)) < 0) {
        std::cerr << "write failed\n";
        close(acceptSocket);
        break;
      }
    }
  }

  return 0;
}