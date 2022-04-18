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

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/Commands/replaceMapEntryCommand.hpp>

babelwires::MapModelDelegate::MapModelDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

QWidget* babelwires::MapModelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                                    const QModelIndex& index) const {
    const MapModel* const mapModel = qobject_cast<const MapModel*>(index.model());
    assert(mapModel && "Unexpected model");

    MapEntryModelDispatcher mapEntryModel;
    mapModel->initMapEntryModelDispatcher(index, mapEntryModel);

    assert(mapEntryModel->isItemEditable(static_cast<unsigned int>(index.column())) &&
           "We should not be trying to create an editor for a non-editable feature");

    QWidget* const editor = mapEntryModel->createEditor(index, parent);

    if (editor) {
        QVariant property = editor->property(ValueEditorInterface::s_propertyName);
        if (property.isValid()) {
            ValueEditorInterface* interface = qvariant_cast<ValueEditorInterface*>(property);

            // Update the editor if the model changes.
            interface->getValuesChangedConnection() = QObject::connect(
                mapModel, &MapModel::valuesMayHaveChanged,
                [this, editor, parent, index]() { emit setEditorData(editor, index); });

            ValueEditorCommonSignals* ValueEditorCommonSignals = interface->getValueEditorSignals();
            // Update the model if the editor changes.
            QObject::connect(ValueEditorCommonSignals, &ValueEditorCommonSignals::editorHasChanged,
                     this, &MapModelDelegate::commitData);
        }
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

    unsigned int column = static_cast<unsigned int>(index.column());

    assert(mapEntryModel->isItemEditable(column) &&
           "We should not be trying to create an editor for a non-editable feature");

    mapEntryModel->setEditorData(column, editor);
}

void babelwires::MapModelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                const QModelIndex& index) const {
    const MapModel* const mapModel = qobject_cast<const MapModel*>(index.model());
    assert(mapModel && "Unexpected model");
    
    MapEntryModelDispatcher mapEntryModel;
    mapModel->initMapEntryModelDispatcher(index, mapEntryModel);

    unsigned int column = static_cast<unsigned int>(index.column());
    unsigned int row = static_cast<unsigned int>(index.row());

    std::unique_ptr<MapEntryData> replacementData = mapEntryModel->createReplacementDataFromEditor(column, editor);
    std::string editType = (column == 0) ? "key" : "value";
    auto command = std::make_unique<ReplaceMapEntryCommand>("Set map entry " + editType, std::move(replacementData), row);
    mapModel->getMapEditor().executeCommand(std::move(command));
}
