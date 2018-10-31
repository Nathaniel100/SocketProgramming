//
// Created by 吴凡 on 2018/10/30.
//

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <socket/socket.h>
#include <string.h>

#include "socket/log.h"
#include "socket/socket.h"

namespace sock {

static const int kListenQ = 20;

Socket::Socket(int s) : socket_(s) {}

Result Socket::Recv(size_t size, void *buf, size_t *nread) {
  int r = static_cast<int>(recv(socket_, buf, size, 0));
  if (r < 0) {
    LOG_E("recv failed: %s", strerror(errno));
    return {ERROR_RECV, "recv failed"};
  } else if (r == 0) {
    LOG_E("Peer disconnected");
    return {ERROR_RECV_EOF, "eof"};
  }
  if (nread) {
    *nread = static_cast<size_t>(r);
  }
  return {};
}
Result Socket::Send(const void *buf, size_t size, size_t *nwrite) {
  int r = static_cast<int>(send(socket_, buf, size, 0));
  if (r <= 0) {
    LOG_E("send failed: %s", strerror(errno));
    return {ERROR_SEND, "send failed"};
  }
  if (nwrite) {
    *nwrite = static_cast<size_t>(r);
  }
  return {};
}
Result Socket::Close() {
  close(socket_);
  return {};
}
Result Socket::Accept(Socket *client_socket) {
  int socket_tcp = accept(socket_, nullptr, nullptr);
  if (socket_tcp < 0) {
    LOG_E("Accept failed: %s", strerror(errno));
    return {ERROR_ACCEPT, "accept failed"};
  }
  if (client_socket) {
    *client_socket = Socket(socket_tcp);
  }
  return {};
}

Result SetAddress(struct sockaddr_in *addr, const char *host, int port) {
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  if (inet_pton(AF_INET, host, &addr->sin_addr) <= 0) {
    LOG_E("inet_pton failed: %s", strerror(errno));
    return {ERROR_ADDRESS, "inet_pton failed"};
  }
  return {};
}
Result CreateClientSocket(const char *server_address, int server_port,
                          Socket *client_socket) {
  int socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_tcp < 0) {
    LOG_E("Socket create failed: %s", strerror(errno));
    return {ERROR_SOCKET, "socket create failed"};
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  Result result = SetAddress(&addr, server_address, server_port);
  if (!result) {
    LOG_E("SetAddress failed (%d): %s", result.code(), result.message());
    return result;
  }
  if (connect(socket_tcp, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    LOG_E("connect failed: %s", strerror(errno));
    return {ERROR_CONNECT, "connect failed"};
  }
  if (client_socket) {
    *client_socket = Socket(socket_tcp);
  }
  return {};
}
Result CreateServerSocket(const char *address, int port,
                          Socket *server_socket) {
  int socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_tcp < 0) {
    LOG_E("Socket create failed: %s", strerror(errno));
    return {ERROR_SOCKET, "socket create failed"};
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  Result result = SetAddress(&addr, address, port);
  if (!result) {
    LOG_E("SetAddress failed (%d): %s", result.code(), result.message());
    return result;
  }

  if (bind(socket_tcp, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    LOG_E("Bind failed: %s", strerror(errno));
    return {ERROR_BIND, "bind failed"};
  }
  if (listen(socket_tcp, kListenQ) < 0) {
    LOG_E("Listen failed: %s", strerror(errno));
    return {ERROR_LISTEN, "listen failed"};
  }
  if (server_socket) {
    *server_socket = Socket(socket_tcp);
  }
  return {};
}

}  // namespace sock
