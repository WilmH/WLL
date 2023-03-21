cmake_minimum_required(VERSION 3.1)
project(WLL VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)

set(ZIP_NAME "wll.${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.zip")
set(ZIP_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/include/wll.h
)
# Create build directory for zip file
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${ZIP_NAME}" ${ZIP_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build
    COMMENT "Creating zip file of project"
)

configure_file(include/wll.h build/wll.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/build/wll.h DESTINATION include)

