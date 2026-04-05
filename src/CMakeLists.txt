# --- Core library (FBX SDK layer, Qt6::Core only) ---
add_library(fbxcore_lib STATIC
    core/FbxFileIO.cpp
    core/FbxFileIO.h
    core/FbxDocument.cpp
    core/FbxDocument.h
    core/FbxEnums.h
    core/FbxPropertyBridge.cpp
    core/FbxPropertyBridge.h
    core/FbxSceneData.cpp
    core/FbxSceneData.h
)

target_include_directories(fbxcore_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fbxcore_lib PUBLIC
    FBXSDK::FBXSDK
    Qt6::Core
)

# --- Model library (Qt model/view adapters) ---
add_library(model_lib STATIC
    model/NodeTreeModel.cpp
    model/NodeTreeModel.h
    model/PropertyTreeModel.cpp
    model/PropertyTreeModel.h
    model/UserPropertyModel.cpp
    model/UserPropertyModel.h
)

target_link_libraries(model_lib PUBLIC
    fbxcore_lib
    Qt6::Core
)

# --- Command library (Undo/Redo) ---
add_library(command_lib STATIC
    command/EditPropertyCommand.cpp
    command/EditPropertyCommand.h
    command/EditSceneInfoCommand.cpp
    command/EditSceneInfoCommand.h
)

target_link_libraries(command_lib PUBLIC
    fbxcore_lib
    Qt6::Widgets
)

# --- UI library (Qt Widgets) ---
add_library(ui_lib STATIC
    ui/MainWindow.cpp
    ui/MainWindow.h
    ui/NodeTreePanel.cpp
    ui/NodeTreePanel.h
    ui/PropertyPanel.cpp
    ui/PropertyPanel.h
    ui/PropertyDelegate.cpp
    ui/PropertyDelegate.h
    ui/Vec3EditWidget.cpp
    ui/Vec3EditWidget.h
    ui/SceneInfoDialog.cpp
    ui/SceneInfoDialog.h
)

target_link_libraries(ui_lib PUBLIC
    model_lib
    command_lib
    Qt6::Widgets
)

# --- Executable ---
add_executable(HalbyFbxEditor WIN32
    main.cpp
)

target_link_libraries(HalbyFbxEditor PRIVATE
    ui_lib
)

# Copy Qt platform plugin to output directory
# vcpkg places release DLLs under <prefix>/Qt6/plugins/platforms/
# and debug DLLs under <prefix>/debug/Qt6/plugins/platforms/
get_target_property(_qt6_core_loc Qt6::Core LOCATION)
cmake_path(GET _qt6_core_loc PARENT_PATH _qt6_bin_dir)
cmake_path(GET _qt6_bin_dir PARENT_PATH _qt6_prefix)

add_custom_command(TARGET HalbyFbxEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HalbyFbxEditor>/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${_qt6_prefix}/debug/Qt6/plugins/platforms/qwindowsd.dll,${_qt6_prefix}/Qt6/plugins/platforms/qwindows.dll>"
        "$<TARGET_FILE_DIR:HalbyFbxEditor>/platforms/"
)

# Copy FBX SDK DLL to output directory
add_custom_command(TARGET HalbyFbxEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${FBXSDK_DLL_DEBUG},${FBXSDK_DLL_RELEASE}>"
        "$<TARGET_FILE_DIR:HalbyFbxEditor>/"
)
