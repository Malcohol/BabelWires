SET( BABELWIRESQTUI_SRCS
    uiMain.cpp
    mainWindow.cpp
    LogWindow/logWindow.cpp
    ModelBridge/nodeContentsView.cpp
    ModelBridge/nodeContentsModel.cpp
    ModelBridge/rowModelDelegate.cpp
    NodeEditorBridge/modifyModelScope.cpp
    NodeEditorBridge/accessModelScope.cpp
    ModelBridge/RowModels/fileRowModel.cpp
    ModelBridge/RowModels/rowModel.cpp
    ModelBridge/RowModels/rowModelDispatcher.cpp
    ModelBridge/RowModels/valueRowModel.cpp
    ValueEditors/dropDownValueEditor.cpp
    ValueEditors/spinBoxValueEditor.cpp
    ValueEditors/lineEditValueEditor.cpp
    ValueEditors/tupleValueEditor.cpp
    ValueEditors/typeWidget.cpp
    ValueEditors/valueEditorCommonBase.cpp
    NodeEditorBridge/NodeFactories/nodeFactory.cpp
    NodeEditorBridge/NodeFactories/processorNodeFactory.cpp
    NodeEditorBridge/NodeFactories/sourceFileNodeFactory.cpp
    NodeEditorBridge/NodeFactories/targetFileNodeFactory.cpp
    NodeEditorBridge/NodeFactories/valueNodeFactory.cpp
    NodeEditorBridge/nodeNodeModel.cpp
    NodeEditorBridge/projectGraphicsScene.cpp
    NodeEditorBridge/projectGraphModel.cpp
    ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.cpp
    ModelBridge/ContextMenu/reloadFileAction.cpp
    ModelBridge/ContextMenu/changeSourceFileAction.cpp
    ModelBridge/ContextMenu/projectCommandContextMenuAction.cpp
    ModelBridge/ContextMenu/saveFileAction.cpp
    ModelBridge/ContextMenu/saveFileAsAction.cpp
    ModelBridge/ContextMenu/setArraySizeAction.cpp
    ModelBridge/ContextMenu/openValueEditorAction.cpp
    ModelBridge/ContextMenu/setTypeVariableAction.cpp
    Utilities/colours.cpp
    Utilities/fileDialogs.cpp
    Utilities/qtUtils.cpp
    ComplexValueEditors/complexValueEditor.cpp
    ComplexValueEditors/complexValueEditorFactory.cpp
    ComplexValueEditors/complexValueEditorManager.cpp
    ComplexValueEditors/MapEditor/mapEditor.cpp
    ComplexValueEditors/MapEditor/mapModel.cpp
    ComplexValueEditors/MapEditor/mapModelDelegate.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/oneToOneMapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/allToSameFallbackMapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapContextMenuActions/mapCommandContextMenuAction.cpp
    ContextMenu/contextMenu.cpp
    ContextMenu/contextMenuAction.cpp
    Dialogs/messageBox.cpp
    Dialogs/typeInputDialog.cpp
    ValueModels/arrayValueModel.cpp
    ValueModels/enumValueModel.cpp
    ValueModels/genericTypeValueModel.cpp
    ValueModels/intValueModel.cpp
    ValueModels/rationalValueModel.cpp
    ValueModels/recordValueModel.cpp
    ValueModels/recordWithVariantsValueModel.cpp
    ValueModels/stringValueModel.cpp
    ValueModels/tupleValueModel.cpp
    ValueModels/valueModel.cpp
    ValueModels/valueModelDispatcher.cpp
    ValueModels/valueModelRegistry.cpp
    ValueModels/valueModelWithRichText.cpp
    ValueModels/mapValueModel.cpp
    ValueModels/sumValueModel.cpp
)

ADD_LIBRARY( BabelWiresQtUi ${BABELWIRESQTUI_SRCS} )

TARGET_COMPILE_DEFINITIONS(BabelWiresQtUi PRIVATE NODE_EDITOR_STATIC)
TARGET_INCLUDE_DIRECTORIES(BabelWiresQtUi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
TARGET_LINK_LIBRARIES( BabelWiresQtUi BabelWiresLib BaseLib QtNodes Qt6::Core Qt6::Widgets Qt6::Gui Qt6::OpenGL)