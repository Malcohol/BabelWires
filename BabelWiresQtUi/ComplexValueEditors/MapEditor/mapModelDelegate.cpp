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

    QWidget* const result = mapEntryModel->createEditor(index, parent);

    if (!result) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    return result;
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

    // TODO
}
