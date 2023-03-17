
cmake_minimum_required(VERSION 3.1)
project(WLL)

set(CMAKE_C_STANDARD 99)

add_library(wll STATIC src/wll.c)
target_include_directories(wll PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS wll DESTINATION lib)
install(FILES include/wll.h DESTINATION include)

