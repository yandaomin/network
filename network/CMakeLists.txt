cmake_minimum_required(VERSION 3.10)


if (WIN32)
	#project(network VERSION 1.0)
	project(network)
	# 解决编译器堆溢出的问题
	set(CMAKE_GENERATOR_TOOLSET "host=x64")
elseif (UNIX)
	PROJECT(network)
endif()

set(SRCDIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(LIBDIR ${PROJECT_SOURCE_DIR}/lib)
if (WIN32)
	string(REPLACE "/" "\\" SRCDIR "${SRCDIR}")
	string(REPLACE "/" "\\" INCLUDE_DIR "${INCLUDE_DIR}")
	string(REPLACE "/" "\\" LIBDIR "${LIBDIR}")
endif()


#采用utf-8
#add_definitions(-DUNICODE -D_UNICODE)
#设置标志位 dll_output
add_definitions(-DNETWORK_EXPORTS)
#获取所有文件
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
file(GLOB_RECURSE NETWORK_SOURCES ${SRCDIR}/*.cpp )
file(GLOB_RECURSE NETWORK_HEADERS ${SRCDIR}/*.h ${SRCDIR}/*.hxx ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.hxx)
SET(ALL_NETWORK_FILES ${NETWORK_SOURCES} ${NETWORK_HEADERS})
if(WIN32)
	string(REPLACE "\\" "/" ALL_NETWORK_FILES "${ALL_NETWORK_FILES}")
endif()

if(WIN32)
	#文件分组
	foreach(FILE ${ALL_NETWORK_FILES})
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		string(REPLACE "/" "\\" GROUP "${GROUP}")
		if ("${FILE}" MATCHES ".*\\.cpp" OR "${FILE}" MATCHES ".*\\.cc")
			set(GROUP "Source Files${GROUP}")
		elseif("${FILE}" MATCHES ".*\\.h"  OR "${FILE}" MATCHES ".*\\.hxx")
			set(GROUP "Header Files${GROUP}")
		endif()
		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endif()

if(UNIX)
	LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/libuv1.40.0/lib/linux)
endif()


add_library(${PROJECT_NAME} SHARED ${ALL_NETWORK_FILES} )
#add_executable(${PROJECT_NAME} ${ALL_NETWORK_FILES} )

INCLUDE_DIRECTORIES(${LIBDIR}/libuv1.40.0/include)

#编译参数
if(WIN32)
	if (MSVC)
		#target_compile_features(network PRIVATE cxx_std_17 )
		#set(CMAKE_BUILD_TYPE "Release")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  /ignore:4099")

		# Build with multiple processes
		add_definitions(/MP)

		# release也生成调试信息
		add_definitions(/Zi)
		#SET(CMAKE_EXE_LINKER_FLAGS  "/DEBUG")
		#SET(CMAKE_EXE_LINKER_FLAGS  "/DEBUG:FULL")
		#SET(CMAKE_EXE_LINKER_FLAGS  "/DEBUG:FULL /OPT:REF /OPT:ICF" )
		SET(CMAKE_EXE_LINKER_FLAGS  "/DEBUG:FULL /OPT:REF /OPT:ICF" )
		####################set vs solution properties
		# 临时设置关闭release编译优化，方便debug
		#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
		#SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Od")
		#SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} debug ${LIBDIR}\\libuv1.40.0\\lib\\windows\\debug\\uv_a.lib 
											  optimized ${LIBDIR}\\libuv1.40.0\\lib\\windows\\release\\uv_a.lib)
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} ws2_32.lib Psapi.lib userenv.lib Iphlpapi.lib
				kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib
				ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib)
	endif (MSVC)
elseif(UNIX)
	LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/libuv1.40.0/lib/linux)
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE "libuv_a.a" -pthread)
	#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O2")
	#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -O2 ")

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
	    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -Wall -O0 -g -ggdb")
	    #SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -O0 -Wall -g -ggdb")
	    #SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -Wall -O0 -g -fPIC")
	elseif (CMAKE_BUILD_TYPE MATCHES "Release")
	    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -Wall -O2")
	else()
	endif()
endif()

#link libuv
INCLUDE_DIRECTORIES(${LIBDIR}/libuv1.40.0/include)

target_include_directories(${PROJECT_NAME} PUBLIC "${INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${SRCDIR}/network")
target_include_directories(${PROJECT_NAME} PUBLIC "${SRCDIR}/app")
target_include_directories(${PROJECT_NAME} PUBLIC "${SRCDIR}/utility")