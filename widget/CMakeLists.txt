SET(CAL_WIDGET calendar-widget)

FILE(GLOB SRCS
	${CMAKE_SOURCE_DIR}/widget/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/edit/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/common/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/month/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/list/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/book/*.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/common/fw
	${CMAKE_SOURCE_DIR}/common/model
	${CMAKE_SOURCE_DIR}/main-app/src/edit
	${CMAKE_SOURCE_DIR}/main-app/src/common
	${CMAKE_SOURCE_DIR}/main-app/src/month
	${CMAKE_SOURCE_DIR}/main-app/src/list
	${CMAKE_SOURCE_DIR}/main-app/src/book
)

ADD_EXECUTABLE(${CAL_WIDGET} ${SRCS})
TARGET_LINK_LIBRARIES(${CAL_WIDGET} app-assist-efl calendar-common ${pkgs_LDFLAGS} "-pie -lm")

#edj compile

INSTALL(TARGETS ${CAL_WIDGET} DESTINATION ${BINDIR})
