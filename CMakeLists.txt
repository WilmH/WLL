
cmake_minimum_required(VERSION 3.1)
project(WLL VERSION 1.0)

set(CMAKE_C_STANDARD 99)

configure_file(include/wll.h build/wll.h)
install(FILES build/wll.h DESTINATION include)

