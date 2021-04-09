set(TARGET_NAME 3MF)

add_library(${TARGET_NAME} MODULE)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        main.cpp
        utility.cpp
)

# -----------------------------------------------------------------------------
# compiler configuration
# -----------------------------------------------------------------------------
compile_config(${TARGET_NAME})

# -----------------------------------------------------------------------------
# include directories
# -----------------------------------------------------------------------------
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${MAYA_INCLUDE_DIRS}
        ${LIB3MF_INCLUDES}
)

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_libraries(${TARGET_NAME}
    PRIVATE
        ${MAYA_LIBRARIES}
        lib3mf
)

# -----------------------------------------------------------------------------
# properties
# -----------------------------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MAYA_PLUGIN_SUFFIX ".bundle")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(MAYA_PLUGIN_SUFFIX ".mll")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(MAYA_PLUGIN_SUFFIX ".so")
endif()

set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ${MAYA_PLUGIN_SUFFIX})

set(_MAYA_DEFINES REQUIRE_IOSTREAM _BOOL)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(_MAYA_DEFINES "${_MAYA_DEFINES}" MAC_PLUGIN OSMac_ OSMac_MachO)
    set_target_properties(${TARGET_NAME} PROPERTIES
                          PREFIX "")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(_MAYA_DEFINES "${_MAYA_DEFINES}" _AFXDLL _MBCS NT_PLUGIN)
    set_target_properties( ${TARGET_NAME} PROPERTIES
                           LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
else()
    set(_MAYA_DEFINES "${_MAYA_DEFINES}" LINUX LINUX_64)
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
endif()
target_compile_definitions(${TARGET_NAME}
    PRIVATE
        ${_MAYA_DEFINES}
)

# -----------------------------------------------------------------------------
# install
# -----------------------------------------------------------------------------
install(TARGETS ${TARGET_NAME}
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Maya/plugin
    ARCHIVE
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Maya/plugin
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Maya/plugin
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX}/Maya/plugin OPTIONAL)
endif()

# -----------------------------------------------------------------------------
# subdirectories
# -----------------------------------------------------------------------------
add_subdirectory(read)
add_subdirectory(write)

# ---------------------------------------------------------------------------------
# install M3MF.mod
# ---------------------------------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/M3MF.mod.template ${PROJECT_BINARY_DIR}/M3MF.mod)
install(FILES ${PROJECT_BINARY_DIR}/M3MF.mod
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Maya
)
