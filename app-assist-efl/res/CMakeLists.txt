CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# install edj
ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/app-assist-efl.edj
        COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/images ${EDJE_CFLAGS}
        ${CMAKE_CURRENT_SOURCE_DIR}/edje/app-assist-efl.edc
        ${CMAKE_BINARY_DIR}/app-assist-efl.edj
        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/edje/app-assist-efl.edc
)
ADD_CUSTOM_TARGET(app_assist_efl-edj DEPENDS ${CMAKE_BINARY_DIR}/app-assist-efl.edj)
ADD_DEPENDENCIES(${PROJECT_NAME} app_assist_efl-edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/app-assist-efl.edj DESTINATION ${APP_ASSIST_EDJ_PATH})
