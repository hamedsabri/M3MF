if(DEFINED MAXSDK_LOCATION AND BUILD_3DSMAX_PLUGIN AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_subdirectory(3dsmax)
endif()

if (DEFINED MAYA_LOCATION AND BUILD_MAYA_PLUGIN)
    find_package(Maya REQUIRED)
    add_subdirectory(maya)
endif()