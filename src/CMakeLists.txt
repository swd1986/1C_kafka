cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(1C_kafka)

set(1C_kafka_SRC 
    1C_kafka.cpp
    1C_kafka.h
    dllmain.cpp
    stdafx.cpp
    stdafx.h
    )

include_directories(${CMAKE_SOURCE_DIR}/../include)

if (UNIX)
    set_source_files_properties(${1C_kafka_SRC}
                              PROPERTIES COMPILE_FLAGS
                              "-fPIC ")
    if (APPLE)
        add_definitions(-D__APPLE__)
    endif()
else()
    set(AddInDef_SRC 1C_kafka.def)
    set(CMAKE_LINK_DEF_FILE_FLAG 1C_kafka.def)
    add_definitions(-DUNICODE -DWIN32)
    
    # Linking against librdkafka and librdkafka++
    find_library(RDKAFKA_LIBRARY NAMES rdkafka PATH_SUFFIXES lib)
    find_library(RDKAFKAPP_LIBRARY NAMES rdkafka++)
    
    if (NOT RDKAFKA_LIBRARY)
        message(FATAL_ERROR "librdkafka library not found")
    endif ()
    
    if (NOT RDKAFKAPP_LIBRARY)
        message(FATAL_ERROR "librdkafka++ library not found")
    endif ()
    
    add_library(${PROJECT_NAME} SHARED ${1C_kafka_SRC} ${AddInDef_SRC})
    target_link_libraries(${PROJECT_NAME} ${RDKAFKAPP_LIBRARY} ${RDKAFKA_LIBRARY})
endif()

# Linux-specific configurations
if (UNIX AND NOT APPLE)
    # Linking against librdkafka and librdkafka++
    find_package(PkgConfig REQUIRED)
    pkg_search_module(RDKAFKA REQUIRED librdkafka)
    
    add_library(${PROJECT_NAME} SHARED ${1C_kafka_SRC} ${AddInDef_SRC})
    target_link_libraries(${PROJECT_NAME} ${RDKAFKA_LIBRARIES})
endif()

# General configuration
set_target_properties(${PROJECT_NAME} PROPERTIES
   OUTPUT_NAME ${PROJECT_NAME}${MySuffix1}${MySuffix2} 
)