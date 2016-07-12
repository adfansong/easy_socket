project(easy_socket)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(src_list main.cpp SocketBase.cpp SocketLinux.cpp SockAddr.cpp Socket.cpp SocketState.cpp Buffer.cpp SocketWin.cpp)
add_executable(test ${src_list})