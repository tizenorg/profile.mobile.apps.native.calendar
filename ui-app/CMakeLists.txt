cmake_minimum_required(VERSION 2.6)
project(ui-app CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)

include_directories(
	${CMAKE_SOURCE_DIR}/lib-apps-common/inc
	${CMAKE_SOURCE_DIR}/lib-apps-common/res
	${CMAKE_SOURCE_DIR}/lib-calendar/inc
	${CMAKE_SOURCE_DIR}/lib-calendar/res
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} -pie apps-common calendar)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_DIR})

configure_file(tizen-manifest.xml.in ${PACKAGE}.xml)
configure_file(smack.manifest.in ${CMAKE_SOURCE_DIR}/${PACKAGE}.manifest)

install(DIRECTORY shared/res/ DESTINATION ${ICON_DIR})
install(FILES ${PACKAGE}.xml DESTINATION ${MANIFEST_DIR})

add_subdirectory(res)
