cmake_minimum_required(VERSION 2.6)

file(GLOB SRCS "*.cpp")

set(CMAKE_INSTALL_RPATH ${CMAKE_APP_LIB_DIR})
set(COMMON_LIB common)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions("-DDEBUG")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_APP_BIN_DIR})

add_subdirectory(res)
