SET( BABELWIRESQTUI_SRCS
    uiMain.cpp
    mainWindow.cpp
    ModelBridge/sourceFileFactory.cpp
    ModelBridge/targetFileFactory.cpp
    ModelBridge/processorNodeFactory.cpp
    ModelBridge/baseNodeModel.cpp
    ModelBridge/projectBridge.cpp
    ModelBridge/elementNodeModel.cpp
    ModelBridge/featureModel.cpp
    ModelBridge/modifyModelScope.cpp
    ModelBridge/accessModelScope.cpp
    ModelBridge/RowModels/rowModel.cpp
    ModelBridge/RowModels/intRowModel.cpp
    ModelBridge/RowModels/stringRowModel.cpp
    ModelBridge/RowModels/rationalRowModel.cpp
    ModelBridge/RowModels/arrayRowModel.cpp
    ModelBridge/RowModels/fileRowModel.cpp
    ModelBridge/RowModels/rowModelWithRichText.cpp
    ModelBridge/RowModels/rowModelDispatcher.cpp
    ModelBridge/RowModels/rowModelRegistry.cpp
    ModelBridge/ValueEditors/comboBoxValueEditor.cpp
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
    Utilities/fileDialogs.cpp
    LogWindow/logWindow.cpp
)

ADD_LIBRARY( BabelWiresQtUi ${BABELWIRESQTUI_SRCS} )

ADD_LIBRARY(nodes STATIC IMPORTED)
SET_TARGET_PROPERTIES(nodes PROPERTIES INTERFACE_INCLUDE_DIRECTORIES /home/malcohol/MyStuff/Projects/Programming/C++/nodeeditor/include)
SET_PROPERTY(TARGET nodes PROPERTY IMPORTED_LOCATION /home/malcohol/MyStuff/Projects/Programming/C++/nodeeditorBuild/lib/libnodes.a)
TARGET_COMPILE_DEFINITIONS(BabelWiresQtUi PRIVATE NODE_EDITOR_STATIC)

TARGET_LINK_LIBRARIES( BabelWiresQtUi BabelWiresLib Common nodes Qt5::Core Qt5::Widgets Qt5::Gui Qt5::OpenGL)