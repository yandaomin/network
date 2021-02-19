# network
基于libuv 4.0的封装库；

libuv是c语言库，使用起来比较麻烦，且它是非线程安全的网络库，容易因内存问题导致服务端程序崩溃，目前已将容易产生崩溃的点改为自身控制，外部发送消息只需要通过id即可；

在此库中抽象出network层，这样做的好处是屏蔽了libuv，后续可以更换为libevent,asio等库时不会影响应用层代码；

此库参照了github上的一些封装库，在此感谢，本人仅根据自己的需要作相应整合及修改，大家可以用来学习；

# 特性：

1. 高性能的基于c++11 的网络库；
2. 支持Tcp、Udp协议；
3. 跨平台，支持在Windows、Linux下使用；

# 封装类：

## 网络层

### TCP

- **TcpAccepter：**监听器，用于监听新的socket连接，服务器端；
- **TcpConnection：**连接对象，用于收发消息、监听socket断开事件并向上层反馈；目前仅在TcpOnDataCallback回调处曝露，使用时不要存储TcpConnection的指针，这部分已经在TcpServer中控制其释放，以免造成 crash；
- **TcpConnecter：**连接器，连接服务器，客户端；

### UDP

- **UdpNetwork：**udp协议封装，由于UDP是无状态的，所以其只支持收发，没有断开和连接的概念，如果需要知道对方是否在线，建议在应该层做心跳包来解决；

### 工具

- **Timer:** 定时器；



## 应用层

- **TCPServer：** tcp服务器端；

- **TcpClient：** tcp客户端；



# 编译

采用cmake脚本编写；

cmake版本是3.17.2；

### windows编译

**vs2017：** 用cmake gui工具 转为 vs2017+项目文件进行编译；



### linux编译

**codeblocks：**用cmake转为 codeblocks项目进行编译；

命令：cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles"  <u>*CMakeLists.txt所在路径*</u>





