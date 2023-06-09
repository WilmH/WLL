cmake_minimum_required(VERSION 3.1)
project(WLL VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)

set(ZIP_NAME "wll.${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.tar.gz")
set(ZIP_FILES 
    CMakeLists.txt
    LICENSE
    README.md
)
# Create build directory for zip file
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/include)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -E copy ${ZIP_FILES} ${CMAKE_CURRENT_BINARY_DIR}/build
    COMMAND ${CMAKE_COMMAND} -E tar "-czfv" "${CMAKE_CURRENT_SOURCE_DIR}/build/${ZIP_NAME}" ${ZIP_FILES} include/
    COMMENT "Making release..."
)

configure_file(include/wll.h build/include/wll.h)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/build/include/wll.h DESTINATION include)

