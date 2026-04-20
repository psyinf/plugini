include(cmake/setup_cpm.cmake)

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 10.2.1
    OPTIONS
        "FMT_DOC OFF"
        "FMT_TEST OFF"
        "FMT_INSTALL OFF"
)

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    GIT_TAG v1.14.1
    OPTIONS
        "SPDLOG_BUILD_EXAMPLES OFF"
        "SPDLOG_BUILD_TESTS OFF"
        "SPDLOG_INSTALL OFF"
        "SPDLOG_FMT_EXTERNAL ON"
)

if(PLUGINI_ENABLE_TESTING AND IS_STANDALONE_PROJECT)
    CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.5.2
        OPTIONS
            "CATCH_BUILD_TESTING OFF"
            "CATCH_BUILD_EXAMPLES OFF"
            "CATCH_BUILD_EXTRA_TESTS OFF"
            "CATCH_BUILD_FUZZERS OFF"
    )
endif()
