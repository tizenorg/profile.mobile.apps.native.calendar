cmake_minimum_required(VERSION 2.6)
project(calendar CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/res
	${CMAKE_SOURCE_DIR}/lib-apps-common/inc
	${CMAKE_SOURCE_DIR}/lib-apps-common/res
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} apps-common)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_DIR})

add_subdirectory(res/common)
add_subdirectory(res/agenda)

