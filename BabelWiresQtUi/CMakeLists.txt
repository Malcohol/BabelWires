SET( BABELWIRESQTUI_SRCS
    uiMain.cpp
    mainWindow.cpp
    MapEditor/mapEditor.cpp
    ModelBridge/sourceFileNodeFactory.cpp
    ModelBridge/targetFileNodeFactory.cpp
    ModelBridge/processorNodeFactory.cpp
    ModelBridge/baseNodeModel.cpp
    ModelBridge/projectBridge.cpp
    ModelBridge/elementNodeModel.cpp
    ModelBridge/featureModel.cpp
    ModelBridge/modifyModelScope.cpp
    ModelBridge/accessModelScope.cpp
    ModelBridge/RowModels/arrayRowModel.cpp
    ModelBridge/RowModels/enumRowModel.cpp
    ModelBridge/RowModels/fileRowModel.cpp
    ModelBridge/RowModels/intRowModel.cpp
    ModelBridge/RowModels/mapRowModel.cpp
    ModelBridge/RowModels/rationalRowModel.cpp
    ModelBridge/RowModels/recordWithOptionalsRowModel.cpp
    ModelBridge/RowModels/rowModel.cpp
    ModelBridge/RowModels/rowModelDispatcher.cpp
    ModelBridge/RowModels/rowModelRegistry.cpp
    ModelBridge/RowModels/rowModelWithRichText.cpp
    ModelBridge/RowModels/stringRowModel.cpp
    ModelBridge/ValueEditors/dropDownValueEditor.cpp
    ModelBridge/ValueEditors/spinBoxValueEditor.cpp
    ModelBridge/ValueEditors/lineEditValueEditor.cpp
    ModelBridge/Internal/flowSceneConnectionInfo.cpp
    ModelBridge/ContextMenu/featureContextMenu.cpp
    ModelBridge/ContextMenu/removeModifierAction.cpp
    ModelBridge/ContextMenu/insertArrayEntryAction.cpp
    ModelBridge/ContextMenu/removeArrayEntryAction.cpp
    ModelBridge/ContextMenu/reloadFileAction.cpp
    ModelBridge/ContextMenu/changeSourceFileAction.cpp
    ModelBridge/ContextMenu/saveFileAction.cpp
    ModelBridge/ContextMenu/saveFileAsAction.cpp
    ModelBridge/ContextMenu/removeFailedModifiersAction.cpp
    ModelBridge/ContextMenu/optionalActivationAction.cpp
    ModelBridge/ContextMenu/setArraySizeAction.cpp
    ModelBridge/ContextMenu/editMapAction.cpp
    Utilities/fileDialogs.cpp
    LogWindow/logWindow.cpp
)

ADD_LIBRARY( BabelWiresQtUi ${BABELWIRESQTUI_SRCS} )

TARGET_COMPILE_DEFINITIONS(BabelWiresQtUi PRIVATE NODE_EDITOR_STATIC)
TARGET_INCLUDE_DIRECTORIES(BabelWiresQtUi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
TARGET_LINK_LIBRARIES( BabelWiresQtUi BabelWiresLib Common nodes Qt5::Core Qt5::Widgets Qt5::Gui Qt5::OpenGL)