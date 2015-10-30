SET(ALERT calendar-alert)

FILE(GLOB SRCS
    ${CMAKE_SOURCE_DIR}/app-control/alert/*.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/app-control/alert
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/common/fw
	${CMAKE_SOURCE_DIR}/common/model
)

ADD_EXECUTABLE(${ALERT} ${SRCS})
TARGET_LINK_LIBRARIES(${ALERT} app-assist-efl calendar-common ${pkgs_LDFLAGS} "-pie -lm")

#edj compile

INSTALL(TARGETS ${ALERT} DESTINATION ${BINDIR})
