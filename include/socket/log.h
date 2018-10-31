//
// Created by 吴凡 on 2018/10/31.
//

#ifndef SOCKETPROGRAMMING_LOG_H
#define SOCKETPROGRAMMING_LOG_H

#include <cstdio>

#define LOG_D(fmt, ...) \
  printf("[%s:%d]:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define LOG_E(fmt, ...) \
  printf("[%s:%d]:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);

#endif  // SOCKETPROGRAMMING_LOG_H
