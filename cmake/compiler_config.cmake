#------------------------------------------------------------------------------
# compiler flags/definitions
#------------------------------------------------------------------------------
set(GNU_CLANG_FLAGS
    # we want to be as strict as possible
    -Wall
    $<$<BOOL:${BUILD_STRICT_MODE}>:-Werror>
    # optimization
    -msse3
    # disable warnings
    -Wno-deprecated
    -Wno-deprecated-declarations
    -Wno-unused-local-typedefs
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND GNU_CLANG_FLAGS
            -Wrange-loop-analysis
        )
endif()

set(MSVC_FLAGS
    # we want to be as strict as possible
    /W4
    $<$<BOOL:${BUILD_STRICT_MODE}>:/WX>
    # enable two-phase name lookup and other strict checks (binding a non-const reference to a temporary, etc..)
    /permissive-
    # enable pdb generation.
    /Zi
    # standards compliant.
    /Zc:rvalueCast
    # The /Zc:inline option strips out the "arch_ctor_<name>" symbols used for
    # library initialization by ARCH_CONSTRUCTOR starting in Visual Studio 2019,
    # causing release builds to fail. Disable the option for this and later
    # versions.
    #
    # For more details, see:
    # https://developercommunity.visualstudio.com/content/problem/914943/zcinline-removes-extern-symbols-inside-anonymous-n.html
    $<IF:$<VERSION_GREATER_EQUAL:${MSVC_VERSION},1920>,/Zc:inline-,/Zc:inline>
    # enable multiprocessor builds.
    /MP
    # enable exception handling.
    /EHsc
    # enable initialization order as a level 3 warning
    /w35038
    # disable warnings
    /wd4244
    /wd4267
    /wd4273
    /wd4305
    /wd4506
    /wd4996
    /wd4180
    # exporting STL classes
    /wd4251
)

set(MSVC_DEFINITIONS
    # Make sure WinDef.h does not define min and max macros which
    # will conflict with std::min() and std::max().
    NOMINMAX

    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
)

#------------------------------------------------------------------------------
# compiler configuration
#------------------------------------------------------------------------------
# Do not use GNU extension 
# Use -std=c++11 instead of -std=gnu++11
set(CMAKE_CXX_EXTENSIONS OFF)

function(compile_config TARGET)
    target_compile_features(${TARGET} 
        PRIVATE
            cxx_std_17
    )
    if(IS_GNU OR IS_CLANG)
        target_compile_options(${TARGET} 
            PRIVATE
                ${GNU_CLANG_FLAGS}
        )
    elseif(IS_MSVC)
        target_compile_options(${TARGET} 
            PRIVATE
                ${MSVC_FLAGS}
        )
        target_compile_definitions(${TARGET} 
            PRIVATE
                ${MSVC_DEFINITIONS}
        )
    endif()
endfunction()
