cmake_minimum_required(VERSION 3.0)

project(test_client)

option(BUILD_SHARED_LIBS "Build shared instead of static library" OFF)

if(MSVC)
	option(STATIC_CRT "Use static CRT libraries" ON)

	# Rewrite command line flags to use /MT if necessary
	if(STATIC_CRT)
		foreach(flag_var
				CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
				CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif(${flag_var} MATCHES "/MD")
		endforeach(flag_var)
	endif()
endif()

# thirdparty
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/pb/include)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/http)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/rapidjson/include)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/pugixml/include)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/curl/include)

include_directories(${PROJECT_SOURCE_DIR}/../src/base)
include_directories(${PROJECT_SOURCE_DIR}/../src/base/protocol)

# thirdparty
set(PB_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../thirdparty/pb/lib)
set(PUGIXML_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../thirdparty/pugixml/lib)
set(CURL_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../thirdparty/curl/lib)

link_directories(
	${PROJECT_SOURCE_DIR}/build/
	${PB_LIBRARY_DIR} 
	${PUGIXML_LIBRARY_DIR}
	${CURL_LIBRARY_DIR}
)

aux_source_directory(${PROJECT_SOURCE_DIR}/../src/base/protocol SRC_PROTOCOL)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11 -g -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-local-typedefs)
endif()


set(build_target test_client)
SET(EXECUTABLE_OUTPUT_PATH .)

include_directories(.)
aux_source_directory(. SRC_LIST)

add_executable(${build_target} ${SRC_LIST})

target_link_libraries(
    ${build_target}
    base
    protobuf-lite
	curl
)
