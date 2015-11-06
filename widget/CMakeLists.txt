SET(CAL_WIDGET calendar-widget)

add_definitions("-DTIZEN_WIDGET=\"_WIDGET\"")
FILE(GLOB SRCS
	${CMAKE_SOURCE_DIR}/widget/*.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/month/CalMonthControl2.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/month/CalMonthControl.cpp
	${CMAKE_SOURCE_DIR}/main-app/src/month/CalMonthRowControl.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/common/fw
	${CMAKE_SOURCE_DIR}/common/model
)

ADD_EXECUTABLE(${CAL_WIDGET} ${SRCS})
TARGET_LINK_LIBRARIES(${CAL_WIDGET} app-assist-efl calendar-common ${pkgs_LDFLAGS} "-pie -lm")

#edj compile

INSTALL(TARGETS ${CAL_WIDGET} DESTINATION ${BINDIR})
