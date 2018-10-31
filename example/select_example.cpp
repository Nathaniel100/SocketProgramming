//
// Created by 吴凡 on 2018/10/31.
//

#include <sys/select.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>


int main() {
  fd_set rdset;
  struct timeval tval;
  int ret = 0;

  FD_ZERO(&rdset);
  FD_SET(0, &rdset);

  tval.tv_sec = 5;
  tval.tv_usec = 0;

  ret = select(1, &rdset, nullptr, nullptr, &tval);
  if (ret < 0) {
    fprintf(stderr, "select failed: %s\n", strerror(errno));
    return -1;
  } else if (ret == 0) {
    fprintf(stderr, "No data withing 5 seconds\n");
    return -1;
  }
  if (FD_ISSET(0, &rdset)) {
    printf("Data is available\n");
  }
  return 0;
}
