cmake_minimum_required(VERSION 3.18)

project(ha_acc)

file(GLOB_RECURSE SRC_FILE src/*.c)
add_executable(${PROJECT_NAME} ${SRC_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE m)