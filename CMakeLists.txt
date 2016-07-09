project(easy_socket)
set(src_list main.cpp SocketBase.cpp SocketLinux.cpp SockAddr.cpp Socket.cpp SocketState.cpp Buffer.cpp)
add_executable(test ${src_list})