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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(PLUGINI_INSTALL)
    include(CMakePackageConfigHelpers)

    install(TARGETS ${PROJECT_NAME}
        EXPORT pluginiTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(FILES
            PluginBase.hpp
            PluginManager.hpp
            StringUtils.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/plugini
    )
    install(EXPORT pluginiTargets
        FILE pluginiTargets.cmake
        NAMESPACE plugini::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/plugini
    )
endif()
