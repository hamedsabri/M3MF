# - Maya finder module
# This module searches for a valid Maya instalation.
# It searches for Maya's devkit, libraries, executables
# and related paths (scripts)
#
# Variables that will be defined:
# MAYA_FOUND          Defined if a Maya installation has been detected
# MAYA_EXECUTABLE     Path to Maya's executable
# MAYA_<lib>_FOUND    Defined if <lib> has been found
# MAYA_<lib>_LIBRARY  Path to <lib> library
# MAYA_INCLUDE_DIRS   Path to the devkit's include directories
# MAYA_API_VERSION    Maya version (6-8 digits)
# MAYA_APP_VERSION    Maya app version (4 digits)
#

#=============================================================================
# Copyright 2011-2012 Francisco Requena <frarees@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # On OSX, setting MAYA_LOCATION to either the base installation dir (ie,
    # `/Application/Autodesk/maya20xx`), or the Contents folder in the Maya.app dir
    # (ie, `/Application/Autodesk/maya20xx/Maya.app/Contents`) are supported.
    find_path(MAYA_BASE_DIR
            include/maya/MFn.h
        HINTS
            "${MAYA_LOCATION}/../.."
            "$ENV{MAYA_LOCATION}/../.."
            "${MAYA_LOCATION}"
            "$ENV{MAYA_LOCATION}"
            "/Applications/Autodesk/maya2021"
            "/Applications/Autodesk/maya2020"
        DOC
            "Maya installation root directory"
    )
    find_path(MAYA_LIBRARY_DIR
            libOpenMaya.dylib
        HINTS
            "${MAYA_LOCATION}"
            "$ENV{MAYA_LOCATION}"
            "${MAYA_BASE_DIR}"
        PATH_SUFFIXES
            MacOS/
            Maya.app/Contents/MacOS/
        DOC
            "Maya's libraries path"
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_path(MAYA_BASE_DIR
            include/maya/MFn.h
        HINTS
            "${MAYA_LOCATION}"
            "$ENV{MAYA_LOCATION}"
            "/usr/autodesk/maya2021-x64"
            "/usr/autodesk/maya2020-x64"
        DOC
            "Maya installation root directory"
    )
    find_path(MAYA_LIBRARY_DIR
            libOpenMaya.so
        HINTS
            "${MAYA_LOCATION}"
            "$ENV{MAYA_LOCATION}"
            "${MAYA_BASE_DIR}"
        PATH_SUFFIXES
            lib/
        DOC
            "Maya's libraries path"
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    find_path(MAYA_BASE_DIR
            include/maya/MFn.h
        HINTS
            "${MAYA_LOCATION}"
            "$ENV{MAYA_LOCATION}"
            "C:/Program Files/Autodesk/Maya2021"
            "C:/Program Files/Autodesk/Maya2020"
        DOC
            "Maya installation root directory"
    )
    find_path(MAYA_LIBRARY_DIR
            OpenMaya.lib
        HINTS
            "${MAYA_LOCATION}"
            "$ENV{MAYA_LOCATION}"
            "${MAYA_BASE_DIR}"
        PATH_SUFFIXES
            lib/
        DOC
            "Maya's libraries path"
    )
endif()

find_path(MAYA_INCLUDE_DIR
        maya/MFn.h
    HINTS
        "${MAYA_LOCATION}"
        "$ENV{MAYA_LOCATION}"
        "${MAYA_BASE_DIR}"
    PATH_SUFFIXES
        ../../devkit/include/
        include/
    DOC
        "Maya's headers path"
)

find_path(MAYA_LIBRARY_DIR
        OpenMaya
    HINTS
        "${MAYA_LOCATION}"
        "$ENV{MAYA_LOCATION}"
        "${MAYA_BASE_DIR}"
    PATH_SUFFIXES
        ../../devkit/include/
        include/
    DOC
        "Maya's libraries path"
)

list(APPEND MAYA_INCLUDE_DIRS ${MAYA_INCLUDE_DIR})

find_path(MAYA_DEVKIT_INC_DIR
       GL/glext.h
    HINTS
        "${MAYA_LOCATION}"
        "$ENV{MAYA_LOCATION}"
        "${MAYA_BASE_DIR}"
    PATH_SUFFIXES
        ../../devkit/plug-ins/
    DOC
        "Maya's devkit headers path"
)
if(NOT "${MAYA_DEVKIT_INC_DIR}" STREQUAL "MAYA_DEVKIT_INC_DIR-NOTFOUND")
    list(APPEND MAYA_INCLUDE_DIRS ${MAYA_DEVKIT_INC_DIR})
endif()

set(MAYA_LIBS_TO_FIND
    OpenMaya
    OpenMayaAnim
    OpenMayaFX
    OpenMayaRender
    OpenMayaUI
    Image
    Foundation
    IMFbase
    cg
    cgGL
    clew
)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND MAYA_LIBS_TO_FIND tbb_debug)
else()
    list(APPEND MAYA_LIBS_TO_FIND tbb)
endif()

foreach(MAYA_LIB ${MAYA_LIBS_TO_FIND})
    find_library(MAYA_${MAYA_LIB}_LIBRARY
            ${MAYA_LIB}
        HINTS
            "${MAYA_LIBRARY_DIR}"
        DOC
            "Maya's ${MAYA_LIB} library path"
        # NO_CMAKE_SYSTEM_PATH needed to avoid conflicts between
        # Maya's Foundation library and OSX's framework.
        NO_CMAKE_SYSTEM_PATH
    )

    if (MAYA_${MAYA_LIB}_LIBRARY)
        list(APPEND MAYA_LIBRARIES ${MAYA_${MAYA_LIB}_LIBRARY})
    endif()
endforeach()

find_program(MAYA_EXECUTABLE
        maya
    HINTS
        "${MAYA_LOCATION}"
        "$ENV{MAYA_LOCATION}"
        "${MAYA_BASE_DIR}"
    PATH_SUFFIXES
        Maya.app/Contents/bin/
        bin/
    DOC
        "Maya's executable path"
)

if(MAYA_INCLUDE_DIRS AND EXISTS "${MAYA_INCLUDE_DIR}/maya/MTypes.h")
    # Tease the MAYA_API_VERSION numbers from the lib headers
    file(STRINGS ${MAYA_INCLUDE_DIR}/maya/MTypes.h TMP REGEX "#define MAYA_API_VERSION.*$")
    string(REGEX MATCHALL "[0-9]+" MAYA_API_VERSION ${TMP})

    # MAYA_APP_VERSION
    file(STRINGS ${MAYA_INCLUDE_DIR}/maya/MTypes.h MAYA_APP_VERSION REGEX "#define MAYA_APP_VERSION.*$")
    if(MAYA_APP_VERSION)
        string(REGEX MATCHALL "[0-9]+" MAYA_APP_VERSION ${MAYA_APP_VERSION})
    else()
        string(SUBSTRING ${MAYA_API_VERSION} "0" "4" MAYA_APP_VERSION)
    endif()
endif()

# handle the QUIETLY and REQUIRED arguments and set MAYA_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Maya
    REQUIRED_VARS
        MAYA_EXECUTABLE
        MAYA_INCLUDE_DIRS
        MAYA_LIBRARIES
        MAYA_API_VERSION
        MAYA_APP_VERSION
    VERSION_VAR
        MAYA_APP_VERSION
)
