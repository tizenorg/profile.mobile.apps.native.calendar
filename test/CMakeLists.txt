cmake_minimum_required(VERSION 2.6)

file(GLOB TEST_SRC TC/*.cpp)

FILE(GLOB_RECURSE APP_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common/*.cpp
	${CMAKE_SOURCE_DIR}/common/fw/*.cpp
	${CMAKE_SOURCE_DIR}/common/model/*.cpp

	${CMAKE_SOURCE_DIR}/main-app/src/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/alert/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/book/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/common/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/detail/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/edit/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/list/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/main/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/month/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/pick/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/search/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/setting/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/view/*.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/thirdparties/gmock/gtest/include
	${CMAKE_CURRENT_SOURCE_DIR}/thirdparties/gmock/include

	${CMAKE_SOURCE_DIR}/common/
	${CMAKE_SOURCE_DIR}/common/fw/
	${CMAKE_SOURCE_DIR}/common/model/

	${CMAKE_SOURCE_DIR}/main-app/src/
	${CMAKE_SOURCE_DIR}/main-app/src/alert/
	${CMAKE_SOURCE_DIR}/main-app/src/book/
	${CMAKE_SOURCE_DIR}/main-app/src/common/
	${CMAKE_SOURCE_DIR}/main-app/src/detail/
	${CMAKE_SOURCE_DIR}/main-app/src/edit/
	${CMAKE_SOURCE_DIR}/main-app/src/list/
	${CMAKE_SOURCE_DIR}/main-app/src/main/
	${CMAKE_SOURCE_DIR}/main-app/src/month/
	${CMAKE_SOURCE_DIR}/main-app/src/pick/
	${CMAKE_SOURCE_DIR}/main-app/src/search/
	${CMAKE_SOURCE_DIR}/main-app/src/setting/
	${CMAKE_SOURCE_DIR}/main-app/src/view/
)

ADD_SUBDIRECTORY(thirdparties/gmock)

ADD_DEFINITIONS(${pkgs_CFLAGS})
ADD_EXECUTABLE(calendar-test
	${APP_SRCS}
	${TEST_SRC}
)

TARGET_LINK_LIBRARIES(calendar-test
	gmock
	${pkgs_LDFLAGS}
)

install(TARGETS calendar-test DESTINATION ${BINDIR})
