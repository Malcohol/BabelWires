SET( BABELWIRESQTUI_SRCS
    uiMain.cpp
    mainWindow.cpp
    LogWindow/logWindow.cpp
    ModelBridge/sourceFileNodeFactory.cpp
    ModelBridge/targetFileNodeFactory.cpp
    ModelBridge/processorNodeFactory.cpp
    ModelBridge/valueNodeFactory.cpp
    ModelBridge/baseNodeModel.cpp
    ModelBridge/projectBridge.cpp
    ModelBridge/nodeNodeModel.cpp
    ModelBridge/nodeContentsView.cpp
    ModelBridge/nodeContentsModel.cpp
    ModelBridge/rowModelDelegate.cpp
    ModelBridge/modifyModelScope.cpp
    ModelBridge/accessModelScope.cpp
    ModelBridge/RowModels/fileRowModel.cpp
    ModelBridge/RowModels/rowModel.cpp
    ModelBridge/RowModels/rowModelDispatcher.cpp
    ModelBridge/RowModels/valueRowModel.cpp
    ValueEditors/dropDownValueEditor.cpp
    ValueEditors/spinBoxValueEditor.cpp
    ValueEditors/lineEditValueEditor.cpp
    ValueEditors/tupleValueEditor.cpp
    ValueEditors/valueEditorCommonBase.cpp
    ModelBridge/Internal/flowSceneConnectionInfo.cpp
    ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.cpp
    ModelBridge/ContextMenu/removeModifierAction.cpp
    ModelBridge/ContextMenu/insertArrayEntryAction.cpp
    ModelBridge/ContextMenu/removeArrayEntryAction.cpp
    ModelBridge/ContextMenu/reloadFileAction.cpp
    ModelBridge/ContextMenu/changeSourceFileAction.cpp
    ModelBridge/ContextMenu/saveFileAction.cpp
    ModelBridge/ContextMenu/saveFileAsAction.cpp
    ModelBridge/ContextMenu/removeFailedModifiersAction.cpp
    ModelBridge/ContextMenu/optionalActivationAction.cpp
    ModelBridge/ContextMenu/selectVariantAction.cpp
    ModelBridge/ContextMenu/setArraySizeAction.cpp
    ModelBridge/ContextMenu/openValueEditorAction.cpp
    Utilities/fileDialogs.cpp
    ComplexValueEditors/complexValueEditor.cpp
    ComplexValueEditors/complexValueEditorFactory.cpp
    ComplexValueEditors/complexValueEditorManager.cpp
    ComplexValueEditors/MapEditor/mapEditor.cpp
    ComplexValueEditors/MapEditor/typeWidget.cpp
    ComplexValueEditors/MapEditor/mapModel.cpp
    ComplexValueEditors/MapEditor/mapModelDelegate.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/oneToOneMapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapEntryModels/allToSameFallbackMapEntryModel.cpp
    ComplexValueEditors/MapEditor/MapContextMenuActions/addEntryMapContextMenuAction.cpp
    ComplexValueEditors/MapEditor/MapContextMenuActions/removeEntryMapContextMenuAction.cpp
    ComplexValueEditors/MapEditor/MapContextMenuActions/changeEntryKindContextMenuAction.cpp
    ContextMenu/contextMenu.cpp
    ContextMenu/contextMenuAction.cpp
    ValueModels/arrayValueModel.cpp
    ValueModels/enumValueModel.cpp
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
)

ADD_LIBRARY( BabelWiresQtUi ${BABELWIRESQTUI_SRCS} )

TARGET_COMPILE_DEFINITIONS(BabelWiresQtUi PRIVATE NODE_EDITOR_STATIC)
TARGET_INCLUDE_DIRECTORIES(BabelWiresQtUi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
TARGET_LINK_LIBRARIES( BabelWiresQtUi BabelWiresLib Common nodes Qt5::Core Qt5::Widgets Qt5::Gui Qt5::OpenGL)