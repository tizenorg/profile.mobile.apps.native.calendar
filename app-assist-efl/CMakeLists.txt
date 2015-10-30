CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(app-assist-efl C CXX)

SET(APP_ASSIST_LIB_PATH "${CALENDAR_PREFIX}/lib" )
SET(APP_ASSIST_EDJ_PATH ${EDJDIR} )

SET(SRCS
	"src/WApp.cpp"
	"src/WAppEventHandler.cpp"	
	"src/WNaviframe.cpp"
	"src/WView.cpp"
	"src/WWindow.cpp"
	"src/WControl.cpp"
	"src/WMenuPopup.cpp"
	"src/WPopup.cpp"
	"src/WGenlistItem.cpp"
	"src/WEvasObject.cpp"
	"src/WVersion.cpp"
	"src/WUiTimer.cpp"
)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)
INCLUDE(FindPkgConfig)
pkg_check_modules(lib_pkgs REQUIRED
	dlog
	elementary
	edje
	capi-appfw-application
	efl-extension
)

FOREACH(flag ${lib_pkgs_CFLAGS})
	SET(LIB_CFLAGS "${LIB_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(GC_SECTIONS_FLAGS "-fdata-sections -ffunction-sections -Wl,--gc-sections")
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fPIC ${GC_SECTIONS_FLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIB_CFLAGS} ${EXTRA_CFLAGS} -Wl,--no-undefined -Werror-implicit-function-declaration")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIB_CFLAGS} -Wall -g -fvisibility=hidden -Wl,--no-undefined -fPIC -std=c++0x ${GC_SECTIONS_FLAGS}")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
#ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${lib_pkgs_LDFLAGS})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${APP_ASSIST_LIB_PATH})

## comment out the following line to reduce resource size, if page view and sample view are not used
ADD_SUBDIRECTORY(res)
