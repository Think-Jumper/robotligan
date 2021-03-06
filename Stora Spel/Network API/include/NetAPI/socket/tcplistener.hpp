#define WIN32_LEAN_AND_MEAN
#ifndef TCPLISTENER_HPP_
#define TCPLISTENER_HPP_
#include <NetAPI/common.hpp>
#include <NetAPI/socket/tcpclient.hpp>
#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
namespace NetAPI {
namespace Socket {
class EXPORT TcpListener {
 public:
  TcpListener();
  TcpListener(const TcpListener& other) = delete;
  TcpListener& operator=(const TcpListener& other) = delete;
  ~TcpListener();
  bool Bind(const unsigned short port);
  bool Accept(NetAPI::Socket::TcpClient* cli);
  int QuerryError() { return error_; }
  const char* Recv(SOCKET& cli);  // Deprecated
  void Disconnect();
  size_t GetRecvBytes() { return last_bytes_; }

 private:
  size_t last_bytes_;
  fd_set read_set_ = {};
  timeval timeout_ = {};
  bool setup_ = false;
  int error_ = 0;
  unsigned buffer_size_ = Common::kNumPacketBytes;
  char* rec_buffer_;
  SOCKET listen_socket_ = INVALID_SOCKET;
};
}  // namespace Socket
}  // namespace NetAPI
#endif  // !TCPLISTENER_H
