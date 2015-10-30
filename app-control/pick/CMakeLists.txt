SET(CHOOSER calendar-pick)

FILE(GLOB SRCS
    ${CMAKE_SOURCE_DIR}/app-control/pick/*.cpp
  	${CMAKE_SOURCE_DIR}/main-app/src/common/*.cpp
  	${CMAKE_SOURCE_DIR}/main-app/src/list/*.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/app-control/pick
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/common/fw
	${CMAKE_SOURCE_DIR}/common/model
	${CMAKE_SOURCE_DIR}/main-app/src/common
	${CMAKE_SOURCE_DIR}/main-app/src/list
)


ADD_EXECUTABLE(${CHOOSER} ${SRCS})

TARGET_LINK_LIBRARIES(${CHOOSER} app-assist-efl calendar-common ${pkgs_LDFLAGS} "-pie -lm")

#edj compile

INSTALL(TARGETS ${CHOOSER} DESTINATION ${BINDIR})
