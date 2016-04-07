cmake_minimum_required(VERSION 2.6)
project(calendar-app CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)

include_directories(
	${CMAKE_SOURCE_DIR}/lib-apps-common/inc
	${CMAKE_SOURCE_DIR}/lib-apps-common/res
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_SOURCE_DIR}/lib-common/res
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/res
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} -pie apps-common common)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_DIR})

configure_file(tizen-manifest.xml.in ${PACKAGE}.xml)

install(DIRECTORY shared/res/ DESTINATION ${ICON_DIR})
install(FILES ${PACKAGE}.xml DESTINATION ${MANIFEST_DIR})

add_subdirectory(res)
