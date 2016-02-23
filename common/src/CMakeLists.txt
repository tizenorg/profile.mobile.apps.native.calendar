cmake_minimum_required(VERSION 2.6)

file(GLOB SRCS "*.cpp")

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME} ${TIZEN_PKGS_LDFLAGS}
	"apps-common"
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_DIR})
