project(plugini)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
        PluginBase.hpp
        PluginManager.hpp
        StringUtils.hpp
    PRIVATE
        PluginBase.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        fmt::fmt
        spdlog::spdlog
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)
