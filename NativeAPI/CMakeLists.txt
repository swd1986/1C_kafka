# Cmake script for project AddInNative
# autor: 1C

CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

PROJECT(AddInNative)

SET(AddInNative_SRC 
    AddInNative.cpp
    AddInNative.h
    dllmain.cpp
    stdafx.cpp
    stdafx.h
    )

include_directories(${CMAKE_SOURCE_DIR}/../include)

SET (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR})
if (UNIX)
    set_source_files_properties(${AddInNative_SRC}
                              PROPERTIES COMPILE_FLAGS
                              "-fPIC ")
    if (APPLE)
        ADD_DEFINITIONS(-D__APPLE__ )
    endif()
else()
    SET(AddInDef_SRC AddInNative.def)
    SET(CMAKE_LINK_DEF_FILE_FLAG AddInNative.def)
    add_definitions(-DUNICODE -DWIN32 )
endif()
if(APPLE)
    set(BNDL ${PROJECT_NAME}Bndl)

    add_library(${BNDL} MODULE ${AddInNative_SRC} ${AddInDef_SRC})

    set_target_properties(${BNDL} PROPERTIES BUNDLE TRUE)
    set_target_properties( ${BNDL} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
    SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}")
    SET(MACOSX_BUNDLE_COPYRIGHT "Copyright 2018 MyCompany.")
    TARGET_LINK_LIBRARIES(${BNDL} "-framework CoreFoundation")
    set_target_properties(${BNDL} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in)
endif()

add_library(${PROJECT_NAME} SHARED ${AddInNative_SRC} ${AddInDef_SRC})

set_target_properties( ${PROJECT_NAME} PROPERTIES
   OUTPUT_NAME ${PROJECT_NAME}${MySuffix1}${MySuffix2} 
   )
