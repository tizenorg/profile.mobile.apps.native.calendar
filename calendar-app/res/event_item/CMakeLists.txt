cmake_minimum_required(VERSION 2.6)

set(EDJDIR "${RES_DIR}/event_item/edje")
file(GLOB EDCFILES "${CMAKE_CURRENT_SOURCE_DIR}/edje/*.edc")

foreach(EDCFILE ${EDCFILES})
	get_filename_component(EDJFILE ${EDCFILE} NAME_WE)
	set(EDJFILE ${EDJFILE}.edj)

	add_custom_target(${EDJFILE}
		COMMAND edje_cc -id .. ${EDCFILE} ${EDJFILE}
		DEPENDS ${EDCFILE}
	)

	add_dependencies(${PROJECT_NAME} ${EDJFILE})
	install(FILES ${EDJFILE} DESTINATION ${EDJDIR})
endforeach(${EDCFILE})

install(DIRECTORY images/ DESTINATION "${RES_DIR}/event_item/images")

