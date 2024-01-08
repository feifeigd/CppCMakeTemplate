#pragma once

#include <functional>

using SocketEstablishHandler = std::function<void(TcpSocket)>;   // 客户端连接上
using SocketProcessingHandler = std::function<void(TcpSocket&)>;  // 握手阶段的处理
using SocketFailedConnectHandler = std::function<void()>;
