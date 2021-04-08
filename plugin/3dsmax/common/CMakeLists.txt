set(TARGET_NAME CommonUtil)

add_library(${TARGET_NAME} STATIC)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        utility.cpp
        types.h
)

# -----------------------------------------------------------------------------
# compiler configuration
# -----------------------------------------------------------------------------
compile_config(${TARGET_NAME})

target_compile_definitions(${TARGET_NAME}
    PRIVATE
        _USRDLL
        _UNICODE
        UNICODE
)

# -----------------------------------------------------------------------------
# include directories
# -----------------------------------------------------------------------------
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${MAXSDK_LOCATION}/include
        ${LIB3MF_INCLUDES}
)

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_directories(${TARGET_NAME}
    PRIVATE
        ${MAXSDK_LOCATION}/lib/x64/Release
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        lib3mf
        core
        mesh
        geom
        maxutil
        paramblk2
)
