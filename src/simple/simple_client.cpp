//
// Created by 吴凡 on 2017/10/20.
//

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 11789

int main() {
  // TCP socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "socket failed\n";
    return 1;
  }
  sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(SERVER_PORT);

  if (inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr) <= 0) {
    std::cerr << "server address error\n";
    return 1;
  }

  if (connect(clientSocket, (sockaddr *) &serverAddr, sizeof(struct sockaddr)) == -1) {
    std::cerr << "connect failed\n";
    return 1;
  }


  char buf[100] = {0};
  while (std::cin.getline(buf, 100)) {
    std::cout << "<< " << buf << '\n';
    if (write(clientSocket, buf, strlen(buf)) == -1) {
      std::cerr << "write failed\n";
      break;
    }
    int nread = read(clientSocket, buf, 100);
    if (nread == 0) {
      std::cout << "socket is closed\n";
      break;
    } else if (nread > 0) {
      std::cout << ">> " << buf << '\n';
    } else {
      std::cerr << "read failed\n";
      break;
    }
    memset(buf, 0, 100);
  }

  close(clientSocket);
  return 0;
}
