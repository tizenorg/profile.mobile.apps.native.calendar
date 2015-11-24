CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(${REF_APP_NAME} C CXX)

FILE(GLOB SRCS
    main-app/src/*.cpp
    main-app/src/book/*.cpp
    main-app/src/common/*.cpp
    main-app/src/delete/*.cpp
    main-app/src/detail/*.cpp
    main-app/src/edit/*.cpp
    main-app/src/list/*.cpp
    main-app/src/main/*.cpp
    main-app/src/month/*.cpp
    main-app/src/search/*.cpp
    main-app/src/setting/*.cpp
    main-app/src/view/*.cpp
    main-app/src/pick/*.cpp
    main-app/src/alert/*.cpp
)

SET(VENDOR "org.tizen")
SET(VENDOR_3DP ${VENDOR})
SET(CALENDAR ${PROJECT_NAME})
SET(PKGNAME "${VENDOR}.${REF_APP_NAME}")
SET(CALENDAR_PREFIX ${CMAKE_INSTALL_PREFIX})
SET(BINDIR "${CALENDAR_PREFIX}/bin")
SET(RESDIR "${CALENDAR_PREFIX}/res")
SET(SHAREDRESDIR "${CALENDAR_PREFIX}/shared/res")
SET(LOCALEDIR "${RESDIR}/locale")
SET(ICONDIR "${RESDIR}/icons/default/small")
SET(EDJDIR "${RESDIR}/edje")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
        SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/common/fw
	${CMAKE_SOURCE_DIR}/common/model
	${CMAKE_SOURCE_DIR}/main-app/src
	${CMAKE_SOURCE_DIR}/main-app/src/book
	${CMAKE_SOURCE_DIR}/main-app/src/common
	${CMAKE_SOURCE_DIR}/main-app/src/delete
	${CMAKE_SOURCE_DIR}/main-app/src/detail
	${CMAKE_SOURCE_DIR}/main-app/src/edit
	${CMAKE_SOURCE_DIR}/main-app/src/list
	${CMAKE_SOURCE_DIR}/main-app/src/main
	${CMAKE_SOURCE_DIR}/main-app/src/month
	${CMAKE_SOURCE_DIR}/main-app/src/search
	${CMAKE_SOURCE_DIR}/main-app/src/setting
	${CMAKE_SOURCE_DIR}/main-app/src/view
	${CMAKE_SOURCE_DIR}/main-app/src/pick
	${CMAKE_SOURCE_DIR}/main-app/src/alert
)

SET(pkgs_WIDGET_LIST "")
SET(HTML_COMMENT_START "<!--")
SET(HTML_COMMENT_END "-->")
IF(NOT WIDGET_DISABLED)
        SET(pkgs_WIDGET_LIST ${pkgs_WIDGET_LIST} capi-appfw-widget-application)
        SET(HTML_COMMENT_START "")
        SET(HTML_COMMENT_END "")
ENDIF()

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
		dlog
		contacts-service2
		calendar-service2
		ecore-imf
		elementary
		edje
		glib-2.0
		capi-appfw-alarm
		capi-appfw-application
		capi-appfw-preference
		notification
		sqlite3
		capi-appfw-app-manager
		capi-system-system-settings
		capi-media-metadata-extractor
		capi-system-device
		capi-base-utils-i18n
		efl-extension
		capi-ui-efl-util
		${pkgs_WIDGET_LIST}
)


SET(CMAKE_INSTALL_RPATH "${CALENDAR_PREFIX}/lib")


FOREACH(flag ${pkgs_CFLAGS})
        SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -DCAL_PREFIX=${CALENDAR_PREFIX}")

SET(GC_SECTIONS_FLAGS "-fdata-sections -ffunction-sections -Wl,--gc-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wno-unused -Wno-format-extra-args -Wl,--no-undefined ${GC_SECTIONS_FLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -Wno-unused -Wno-format-extra-args -Wl,--no-undefined -fvisibility=hidden -fPIC -std=c++0x ${GC_SECTIONS_FLAGS}")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        ADD_DEFINITIONS("-DCLD_DEBUG")
        ADD_DEFINITIONS("-DCLD_FILE_LOGGING")
ENDIF()

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE(STATUS "add -DTARGET")
ENDIF()

ADD_DEFINITIONS("-DGBS_BUILD")
ADD_DEFINITIONS("-DCALENDAR=\"${CALENDAR}\"")
ADD_DEFINITIONS("-DPACKAGE_NAME=\"${PKGNAME}\"")
ADD_DEFINITIONS("-DCALENDAR_PREFIX=\"${CALENDAR_PREFIX}\"")
ADD_DEFINITIONS("-DRESDIR=\"${RESDIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DEDJDIR=\"${EDJDIR}\"")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} app-assist-efl calendar-common ${pkgs_LDFLAGS} "-pie -lm")

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/${PKGNAME}.manifest ${CMAKE_BINARY_DIR}/${PKGNAME}.manifest)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/${PKGNAME}.xml ${CMAKE_BINARY_DIR}/${PKGNAME}.xml)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/${PKGNAME}.efl ${CMAKE_BINARY_DIR}/${PKGNAME}.efl)

ADD_SUBDIRECTORY(main-app/res/edje)
ADD_SUBDIRECTORY(main-app/res/po)
ADD_SUBDIRECTORY(main-app/shared)
ADD_SUBDIRECTORY(app-assist-efl)
ADD_SUBDIRECTORY(common)

IF(NOT WIDGET_DISABLED)
        ADD_SUBDIRECTORY(widget)
ENDIF()

# install tizen files
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PKGNAME}.xml DESTINATION /usr/share/packages)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PKGNAME}.efl DESTINATION /etc/smack/accesses.d)
INSTALL(FILES LICENSE DESTINATION /usr/share/license RENAME ${PKGNAME})

INSTALL(DIRECTORY main-app/res/images DESTINATION ${RESDIR})

IF("${PLATFORM}" STREQUAL "2.4")
        INSTALL(DIRECTORY DESTINATION ${DATADIR})
ENDIF()

