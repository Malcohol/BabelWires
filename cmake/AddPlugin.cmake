function(babelwires_add_plugin moduleTarget objectTarget)
    set(options)
    set(oneValueArgs ENTRY_SOURCE)
    set(multiValueArgs SOURCES DEPS INCLUDE_DIRS)
    cmake_parse_arguments(BW_PLUGIN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if("${BW_PLUGIN_SOURCES}" STREQUAL "")
        message(FATAL_ERROR "babelwires_add_plugin(${moduleTarget}): SOURCES is required")
    endif()

    if("${BW_PLUGIN_ENTRY_SOURCE}" STREQUAL "")
        message(FATAL_ERROR "babelwires_add_plugin(${moduleTarget}): ENTRY_SOURCE is required")
    endif()

    add_library(${objectTarget} OBJECT ${BW_PLUGIN_SOURCES})
    target_include_directories(${objectTarget}
        PUBLIC
            ${BW_PLUGIN_INCLUDE_DIRS}
    )
    target_link_libraries(${objectTarget}
        PUBLIC
            ${BW_PLUGIN_DEPS}
    )

    if(BUILD_SHARED_LIBS)
        add_library(${moduleTarget} MODULE ${BW_PLUGIN_ENTRY_SOURCE})
        target_link_libraries(${moduleTarget}
            PRIVATE
                ${objectTarget}
                ${BW_PLUGIN_DEPS}
        )
        set_target_properties(${moduleTarget} PROPERTIES
            PREFIX ""
            SUFFIX ".bwplugin"
            LIBRARY_OUTPUT_DIRECTORY "${BABELWIRES_PLUGIN_OUTPUT_DIR}"
            RUNTIME_OUTPUT_DIRECTORY "${BABELWIRES_PLUGIN_OUTPUT_DIR}"
        )
    endif()
endfunction()