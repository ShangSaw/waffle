cmake_minimum_required(VERSION 3.10)
project(JeuKenneth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(server src/server.cpp "src/packets.h")
add_executable(client src/client.cpp "src/packets.h")

target_link_options(client PRIVATE "-mwindows")

target_include_directories(client PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(server PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(server PUBLIC ${PROJECT_SOURCE_DIR}/lib/enet64.lib ws2_32 winmm)
target_link_libraries(client PUBLIC ${PROJECT_SOURCE_DIR}/lib/enet64.lib ${PROJECT_SOURCE_DIR}/lib/SDL3.lib ws2_32 winmm)


