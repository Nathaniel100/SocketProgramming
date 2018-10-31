//
// Created by 吴凡 on 2018/10/30.
//

#ifndef SOCKETPROGRAMMING_RESULT_H
#define SOCKETPROGRAMMING_RESULT_H

namespace sock {

enum ErrorCode{
  ERROR_SOCKET = 1,
  ERROR_CONNECT,
  ERROR_RECV,
  ERROR_RECV_EOF,
  ERROR_SEND,
  ERROR_ADDRESS,
  ERROR_BIND,
  ERROR_LISTEN,
  ERROR_ACCEPT,
  ERROR_SETSOCKOPT,
};

class Result {
 public:
  Result(): code_(0), message_(nullptr) {}
  Result(int code, const char *message): code_(code), message_(message) {}
  bool success() const { return code_ == 0; }
  explicit operator bool() const { return success(); }
  int code() const { return code_; }
  const char *message() const { return message_; }
 private:
  int code_;
  const char *message_;
};

} // namespace sock

#endif // SOCKETPROGRAMMING_RESULT_H
