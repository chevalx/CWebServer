cmake_minimum_required(VERSION 3.12)
project(CWebServer)

set(CMAKE_CXX_STANDARD 14)
include_directories(.)
add_executable(httpserverp
		httpserverp.c
		wrap.c
		wrap.h)
add_executable(httpservert
		httpservert.c
		wrap.c
		wrap.h)
add_executable(httpservers
		httpservers.c
		wrap.c
		wrap.h)
TARGET_LINK_LIBRARIES(httpserverp pthread)
TARGET_LINK_LIBRARIES(httpservert pthread)
TARGET_LINK_LIBRARIES(httpservers pthread)