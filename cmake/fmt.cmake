include(FetchContent)

set(FETCHCONTENT_QUIET ON)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        6.2.1
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
    GIT_CONFIG     advice.detachedHead=false
)

FetchContent_MakeAvailable(fmt)
