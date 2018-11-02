//
// Created by 吴凡 on 2018/10/30.
//

#include <arpa/inet.h>
#include <signal.h>
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
Result Socket::Recvn(size_t size, void *buf, size_t *nread) {
  uint8_t *p = reinterpret_cast<uint8_t *>(buf);
  int left = static_cast<int>(size);
  while (left > 0) {
    int r = static_cast<int>(recv(socket_, p, size, 0));
    if (r < 0) {
      if (r == EINTR) {
        continue;
      }
      LOG_E("recv failed: %s", strerror(errno));
      return {ERROR_RECV, "recv failed"};
    } else if (r == 0) {
      if (nread) {
        *nread = size - left;
      }
      LOG_E("peer disconnected");
      return {ERROR_RECV_EOF, "eof"};
    }
    left -= r;
    p += r;
  }
  if (nread) {
    *nread = static_cast<size_t>(size);
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

static void ConnectAlarm(int) { return; /* Just interrupt connect */ }

Result Socket::ConnectTimeoutBySigalarm(struct sockaddr *addr, int nsec) {
  Result result;
  SigFunc old_sigalarm_func = Signal(SIGALRM, ConnectAlarm);
  alarm(nsec);
  if (connect(socket_, addr, sizeof(*addr)) < 0) {
    if (errno == EINTR) {
      errno = ETIMEDOUT;
    }
    LOG_E("connect failed: %s", strerror(errno));
    result = {ERROR_CONNECT, "connect failed"};
  }
  alarm(0);
  Signal(SIGALRM, old_sigalarm_func);

  return result;
}

SigFunc Signal(int signo, SigFunc func) {
  struct sigaction act, oact;
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
  } else {
#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;
#endif
  }
  if (::sigaction(signo, &act, &oact) < 0) {
    return SIG_ERR;
  }
  return oact.sa_handler;
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
  const int on = 1;
  if (setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    LOG_E("setsockopt SO_REUSEADDR failed: %s", strerror(errno));
    return {ERROR_SETSOCKOPT, "setsockopt SO_REUSEADDR failed"};
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
