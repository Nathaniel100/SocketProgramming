//
// Created by 吴凡 on 2018/10/30.
//

#ifndef SOCKETPROGRAMMING_SOCKET_H
#define SOCKETPROGRAMMING_SOCKET_H

#include <sys/types.h>

#include "socket/result.h"

namespace sock {

class Socket {
 public:
  explicit Socket(int s = -1);
  // 读取size个字符, 不一定可以读取到size个字符，通过nread返回实际读取的长度
  Result Recv(size_t size, void *buf, size_t *nread);
  // 读取size个字符，不够时阻塞直到出错或断开连接
  Result Recvn(size_t size, void *buf, size_t *nread);
  Result Send(const void *buf, size_t size, size_t *nwrite);
  Result Close();
  Result Accept(Socket *client_socket);
  Result ConnectTimeoutBySigalarm(struct sockaddr *addr,
                                  int nsec);

  int fd() const { return socket_; }

 private:
  int socket_;
};

using SigFunc = void (*)(int);
SigFunc Signal(int signo, SigFunc func);
Result SetAddress(struct sockaddr_in *addr, const char *host, int port);
Result CreateClientSocket(const char *server_address, int server_port,
                          Socket *client_socket);
Result CreateServerSocket(const char *address, int port, Socket *server_socket);

}  // namespace sock

#endif  // SOCKETPROGRAMMING_SOCKET_H
