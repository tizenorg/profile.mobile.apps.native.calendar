SET(VCS calendar-view)

FILE(GLOB SRCS
	${CMAKE_SOURCE_DIR}/app-control/view/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/common/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/edit/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/detail/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/list/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/month/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/book/*.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/app-control/view
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/common/fw
	${CMAKE_SOURCE_DIR}/common/model
	${CMAKE_SOURCE_DIR}/main-app/src/common
	${CMAKE_SOURCE_DIR}/main-app/src/edit
	${CMAKE_SOURCE_DIR}/main-app/src/detail
	${CMAKE_SOURCE_DIR}/main-app/src/list
	${CMAKE_SOURCE_DIR}/main-app/src/month
	${CMAKE_SOURCE_DIR}/main-app/src/book
)

ADD_EXECUTABLE(${VCS} ${SRCS})
TARGET_LINK_LIBRARIES(${VCS} app-assist-efl calendar-common ${pkgs_LDFLAGS} "-pie -lm")

#edj compile

INSTALL(TARGETS ${VCS} DESTINATION ${BINDIR})
