cmake_minimum_required(VERSION 3.10)
project(JeuKenneth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories("src/shared/")

file(GLOB_RECURSE CLIENT_SOURCES
     "${CMAKE_SOURCE_DIR}/src/client/**.cpp"
     "${CMAKE_SOURCE_DIR}/src/client/**.hpp"
     "${CMAKE_SOURCE_DIR}/src/shared/**.cpp"
     "${CMAKE_SOURCE_DIR}/src/shared/**.hpp"
	 "${CMAKE_SOURCE_DIR}/imgui/imgui.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/imgui_tables.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_sdlrenderer3.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_sdl3.cpp"
	 "${CMAKE_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"
)

file(GLOB_RECURSE SERVER_SOURCES
     "${CMAKE_SOURCE_DIR}/src/server/**.cpp"
     "${CMAKE_SOURCE_DIR}/src/server/**.hpp"
     "${CMAKE_SOURCE_DIR}/src/shared/**.cpp"
     "${CMAKE_SOURCE_DIR}/src/shared/**.hpp"
)

add_executable(client ${CLIENT_SOURCES})
add_executable(server ${SERVER_SOURCES})

target_link_options(client PRIVATE "-mwindows")

target_include_directories(client PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/imgui ${PROJECT_SOURCE_DIR}/imgui/backends  ${PROJECT_SOURCE_DIR}/imgui/misc/cpp)
target_include_directories(server PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(server PUBLIC
	${PROJECT_SOURCE_DIR}/lib/enet64.lib
	ws2_32
	winmm
)

target_link_libraries(client PUBLIC
	${PROJECT_SOURCE_DIR}/lib/enet64.lib
	${PROJECT_SOURCE_DIR}/lib/SDL3.lib
	${PROJECT_SOURCE_DIR}/lib/SDL3_image.lib
	${PROJECT_SOURCE_DIR}/lib/SDL3_ttf.lib
	${PROJECT_SOURCE_DIR}/lib/libcurl.lib
	ws2_32
	winmm
	wldap32
)

