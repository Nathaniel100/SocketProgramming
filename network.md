# 网络编程

## 1. 基础知识

### 1.1 面向连接和无连接

面向连接和无连接是协议上的不同，面向连接维护了分组的状态信息，无连接分组是相互独立的。

### 1.2 子网

网络ID + 主机ID

- A类 0XXXXXXX | XXXXXXXX XXXXXXXX XXXXXXXX  0.0.0.1 ~ 127.255.255.255
- B类 10XXXXXX XXXXXXXX | XXXXXXXX XXXXXXXX  128.0.0.0 ~ 191.255.255.255
- C类 110XXXXX XXXXXXXX XXXXXXXX | XXXXXXXX  192.0.0.0 ~ 223.255.255.255
- D类 1110 多播组
- E类 1111 保留

网络ID + 子网 + 主机ID

地址: 192.50.7.75
子网掩码: 255.255.255.192
网络/子网: 192.50.7.64

### 1.2 TCP

### 1.3 UDP

### 1.4 Select

```c
FD_ZERO(fd_set *);
FD_SET(int, fd_set *);
FD_ISSET(int, fd_set *);
FD_CLR(int, fd_set *);
// 如果返回小于0， 则select出错；如果返回0，则表示超时；
int select(int maxfdp1, fd_set *rdset, fd_set *wrset, fd_set *errset, struct timeval *timeout);
```
