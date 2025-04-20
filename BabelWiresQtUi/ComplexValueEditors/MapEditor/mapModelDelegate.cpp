/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModelDelegate.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/Commands/replaceMapEntryCommand.hpp>

babelwires::MapModelDelegate::MapModelDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

QWidget* babelwires::MapModelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                                    const QModelIndex& index) const {
    const MapModel* const mapModel = qobject_cast<const MapModel*>(index.model());
    assert(mapModel && "Unexpected model");

    MapEntryModelDispatcher mapEntryModel;
    mapModel->initMapEntryModelDispatcher(index, mapEntryModel);

    assert(mapEntryModel->isItemEditable() &&
           "We should not be trying to create an editor for a non-editable map");

    QWidget* const editor = mapEntryModel->createEditor(parent, index);

    if (editor) {
        ValueEditorInterface& interface = ValueEditorInterface::getValueEditorInterface(editor);
        interface.getValuesChangedConnection() = QObject::connect(
            mapModel, &MapModel::valuesMayHaveChanged, this,
                [this, editor, index]() { checkEditorIsValid(editor, index); } );

        ValueEditorCommonSignals* ValueEditorCommonSignals = interface.getValueEditorSignals();
        // Update the model if the editor changes.
        QObject::connect(ValueEditorCommonSignals, &ValueEditorCommonSignals::editorHasChanged,
                    this, &MapModelDelegate::commitData);
    }

    if (!editor) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    return editor;
}

void babelwires::MapModelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    const MapModel* const mapModel = qobject_cast<const MapModel*>(index.model());
    assert(mapModel && "Unexpected model");

    MapEntryModelDispatcher mapEntryModel;
    mapModel->initMapEntryModelDispatcher(index, mapEntryModel);

    assert(mapEntryModel->isItemEditable() &&
           "We should not be trying to create an editor for a non-editable map");

    mapEntryModel->setEditorData(editor);
}

void babelwires::MapModelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                const QModelIndex& index) const {
    const MapModel* const mapModel = qobject_cast<const MapModel*>(index.model());
    assert(mapModel && "Unexpected model");
    
    MapEntryModelDispatcher mapEntryModel;
    mapModel->initMapEntryModelDispatcher(index, mapEntryModel);

    unsigned int row = static_cast<unsigned int>(index.row());

    if (std::unique_ptr<MapEntryData> replacementData = mapEntryModel->createReplacementDataFromEditor(editor)) {
        const std::string editType = (mapEntryModel->m_column == MapEntryModel::Column::sourceValue) ? "key" : "value";
        auto command = std::make_unique<ReplaceMapEntryCommand>("Set map entry " + editType, std::move(replacementData), row);
        mapModel->getMapEditor().executeCommand(std::move(command));
    }
}

void babelwires::MapModelDelegate::checkEditorIsValid(QWidget* editor, const QModelIndex& index) const {
        const MapModel* const mapModel = qobject_cast<const MapModel*>(index.model());
    assert(mapModel && "Unexpected model");
    
    MapEntryModelDispatcher mapEntryModel;
    mapModel->initMapEntryModelDispatcher(index, mapEntryModel);

    if (mapEntryModel->isItemEditable() && mapEntryModel->validateEditor(editor)) {
        setEditorData(editor, index);
    } else {
        // This closes an editor without causing it to commit its value.
        const_cast<MapModelDelegate*>(this)->closeEditor(editor);
    }
}
