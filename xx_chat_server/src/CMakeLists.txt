cmake_minimum_required(VERSION 3.0)

project(xx_chat_server)

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
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/mariadb/include)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/http)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/rapidjson/include)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty/pugixml/include)

include_directories(${PROJECT_SOURCE_DIR}/base)
include_directories(${PROJECT_SOURCE_DIR}/base/protocol)

# thirdparty
set(PB_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../thirdparty/pb/lib)
set(MARIADB_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../thirdparty/mariadb/lib)
set(PUGIXML_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../thirdparty/pugixml/lib)

link_directories(
    ${PB_LIBRARY_DIR} 
    ${MARIADB_LIBRARY_DIR}
    ${PUGIXML_LIBRARY_DIR}
)

aux_source_directory(${PROJECT_SOURCE_DIR}/base/protocol SRC_PROTOCOL)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
endif()


# build libs
add_subdirectory(base)

# servers
add_subdirectory(login_server)
add_subdirectory(msg_server)
add_subdirectory(route_server)
add_subdirectory(db_proxy_server)