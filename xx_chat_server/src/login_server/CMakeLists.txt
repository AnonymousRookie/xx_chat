project(login_server)

set(build_target login_server)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../build/run)

include_directories(.)
include_directories(..)

aux_source_directory(. SRC_LIST)
aux_source_directory(../net NET_SRC_LIST)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11 -g -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-local-typedefs)
endif()

add_executable(${build_target} ${SRC_LIST} ${NET_SRC_LIST})

target_link_libraries(
    ${build_target}
    base
    protobuf-lite
)
