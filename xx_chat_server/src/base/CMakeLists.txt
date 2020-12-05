project(base)

set(build_target base)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../build/run)

include_directories(.)

aux_source_directory(. SRC_LIST_BASE)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../thirdparty/http HTTP_PARSER)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11 -g -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-local-typedefs)
endif()

if(BUILD_SHARED_LIBS)
    add_library(${build_target} SHARED ${SRC_LIST_BASE} ${HTTP_PARSER} ${SRC_PROTOCOL})
else()
    add_library(${build_target} STATIC ${SRC_LIST_BASE} ${HTTP_PARSER} ${SRC_PROTOCOL})
endif()

target_link_libraries(${build_target} protobuf-lite)

