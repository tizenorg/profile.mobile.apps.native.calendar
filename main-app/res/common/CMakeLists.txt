cmake_minimum_required(VERSION 2.6)
set(EDJE_CMDLINE_DEFINE "-DGBS_BUILD")
set(EDJDIR "${RES_DIR}/common/edje")
set(EDJFILE main)

add_custom_target(${EDJFILE}.edc
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/images
		${CMAKE_CURRENT_SOURCE_DIR}/edje/${EDJFILE}.edc ${EDJFILE}.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/${EDJFILE}.edc)

add_dependencies(${PROJECT_NAME} ${EDJFILE}.edc)

install(FILES ${EDJFILE}.edj DESTINATION ${EDJDIR})
install(DIRECTORY images/ DESTINATION "${RES_DIR}/common/images")
