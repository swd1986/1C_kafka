# Cmake script for project AddInNative
# autor: 1C

CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

PROJECT(1C_kafka)

SET(1C_kafka_SRC 
    1C_kafka.cpp
    1C_kafka.h
    dllmain.cpp
    stdafx.cpp
    stdafx.h
    )

include_directories(${CMAKE_SOURCE_DIR}/../include)

SET (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR})

	find_package(PkgConfig REQUIRED)
	pkg_check_modules(RDKAFKA REQUIRED rdkafka++)
	include_directories(${RDKAFKA_INCLUDE_DIRS})
	link_directories(${RDKAFKA_LIBRARY_DIRS})

if (UNIX)
    set_source_files_properties(${1C_kafka_SRC}
                              PROPERTIES COMPILE_FLAGS
                              "-fPIC ")
    if (APPLE)
        ADD_DEFINITIONS(-D__APPLE__ )
    endif()
else()
    SET(AddInDef_SRC 1C_kafka.def)
    SET(CMAKE_LINK_DEF_FILE_FLAG 1C_kafka.def)
    add_definitions(-DUNICODE -DWIN32 )
endif()
if(APPLE)
    set(BNDL ${PROJECT_NAME}Bndl)

    add_library(${BNDL} MODULE ${1C_kafka_SRC} ${AddInDef_SRC})

    set_target_properties(${BNDL} PROPERTIES BUNDLE TRUE)
    set_target_properties( ${BNDL} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
    SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}")
    SET(MACOSX_BUNDLE_COPYRIGHT "Copyright 2018 MyCompany.")
    TARGET_LINK_LIBRARIES(${BNDL} "-framework CoreFoundation")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} --lrdkafka++)
    #set_target_properties(${BNDL} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in)
endif()

add_library(${PROJECT_NAME} SHARED ${1C_kafka_SRC} ${AddInDef_SRC})
set_target_properties( ${PROJECT_NAME} PROPERTIES
   OUTPUT_NAME ${PROJECT_NAME}${MySuffix1}${MySuffix2} 
   )
target_link_libraries(${PROJECT_NAME} ${RDKAFKA_LIBRARIES})
