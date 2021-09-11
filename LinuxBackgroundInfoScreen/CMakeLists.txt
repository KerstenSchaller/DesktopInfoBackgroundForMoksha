cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)

project(DesktopbackgroundInfoScreen)

find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)


add_executable(DesktopbackgroundInfoScreen
	main.cpp
)

target_link_libraries(DesktopbackgroundInfoScreen
	PUBLIC X11
)
