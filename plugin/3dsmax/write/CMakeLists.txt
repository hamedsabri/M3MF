set(TARGET_NAME 3MFEXP)

add_library(${TARGET_NAME} MODULE)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        DllEntry.cpp
        ThreeMFExport.cpp
        ThreeMFExport.rc
        ThreeMFExport.def
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
        ${PROJECT_SOURCE_DIR}/plugin/3dsmax/common
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
        CommonUtil
)

# -----------------------------------------------------------------------------
# properties
# -----------------------------------------------------------------------------
set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".dle")

# -----------------------------------------------------------------------------
# install
# -----------------------------------------------------------------------------
install(TARGETS ${TARGET_NAME}
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_PREFIX}/3DSMax/Plugins
    ARCHIVE
    DESTINATION ${CMAKE_INSTALL_PREFIX}/3DSMax/Plugins
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_PREFIX}/3DSMax/Plugins
)

install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX}/3DSMax/Plugins OPTIONAL)

