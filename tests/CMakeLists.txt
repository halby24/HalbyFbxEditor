# Collect DLL directories needed at test runtime
get_target_property(_qt6_core_loc Qt6::Core LOCATION)
cmake_path(GET _qt6_core_loc PARENT_PATH _qt6_bin_dir)

# For multi-config generators (MSVC), the app DLLs land in build/src/Debug or Release.
# We add both so the matching one is found regardless of config.
set(_app_dll_dir_debug   "${CMAKE_BINARY_DIR}/src/Debug")
set(_app_dll_dir_release "${CMAKE_BINARY_DIR}/src/Release")

# Helper: register a Google Test executable
function(add_gtest NAME)
    add_executable(${NAME} ${ARGN})
    target_link_libraries(${NAME} PRIVATE GTest::gtest GTest::gtest_main)
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:${_app_dll_dir_debug};PATH=path_list_prepend:${_app_dll_dir_release};PATH=path_list_prepend:${_qt6_bin_dir}"
    )
endfunction()

# Helper: register a Qt Test executable
function(add_qtest NAME)
    add_executable(${NAME} ${ARGN})
    target_link_libraries(${NAME} PRIVATE Qt6::Test)
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:${_app_dll_dir_debug};PATH=path_list_prepend:${_app_dll_dir_release};PATH=path_list_prepend:${_qt6_bin_dir}"
    )
endfunction()

# ---- Google Test targets ----

add_gtest(test_Vec3 test_Vec3.cpp)
target_link_libraries(test_Vec3 PRIVATE fbxcore_lib)

add_gtest(test_FbxEnums test_FbxEnums.cpp)
target_link_libraries(test_FbxEnums PRIVATE fbxcore_lib)

add_gtest(test_FbxSceneData test_FbxSceneData.cpp)
target_link_libraries(test_FbxSceneData PRIVATE fbxcore_lib)

add_gtest(test_FbxDocument test_FbxDocument.cpp)
target_link_libraries(test_FbxDocument PRIVATE fbxcore_lib)

# ---- Qt Test targets ----

add_qtest(test_NodeTreeModel test_NodeTreeModel.cpp)
target_link_libraries(test_NodeTreeModel PRIVATE model_lib)

add_qtest(test_PropertyTreeModel test_PropertyTreeModel.cpp)
target_link_libraries(test_PropertyTreeModel PRIVATE model_lib)
