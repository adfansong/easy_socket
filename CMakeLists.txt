project(easy_socket)
set(src_list main.cpp SocketBase.cpp SocketLinux.cpp)
add_executable(test ${src_list})