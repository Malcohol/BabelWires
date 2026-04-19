function(add_base_lib_test_plugin target_name source_file)
   add_library(${target_name} MODULE ${source_file})
   target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/BaseLib/generated)
   target_link_libraries(${target_name} PRIVATE BaseLib)
   set_target_properties(${target_name} PROPERTIES
      PREFIX ""
      SUFFIX ".bwplugin"
      LIBRARY_OUTPUT_DIRECTORY "${BASELIB_TEST_PLUGIN_OUTPUT_DIR}"
      RUNTIME_OUTPUT_DIRECTORY "${BASELIB_TEST_PLUGIN_OUTPUT_DIR}"
   )
endfunction()

add_base_lib_test_plugin(baseLibTestPluginGood goodPlugin.cpp)
add_base_lib_test_plugin(baseLibTestPluginBadMagic badMagicPlugin.cpp)
add_base_lib_test_plugin(baseLibTestPluginFingerprintMismatch fingerprintMismatchPlugin.cpp)
add_base_lib_test_plugin(baseLibTestPluginMissingProbe missingProbePlugin.cpp)
add_base_lib_test_plugin(baseLibTestPluginMissingRegistration missingRegistrationPlugin.cpp)