cmake_minimum_required(VERSION 2.6)
project(common CXX)

include_directories(
	${CMAKE_SOURCE_DIR}/app-common/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions("-DCLD_DEBUG")
	add_definitions("-DCLD_FILE_LOGGING")
endif()

find_program(UNAME NAMES uname)
exec_program("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
if("${ARCH}" STREQUAL "arm")
	add_definitions("-DTARGET")
	message(STATUS "add -DTARGET")
endif()

add_subdirectory(src)
add_subdirectory(res/common)
