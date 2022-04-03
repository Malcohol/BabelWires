/**
 * MapEntryModels provide the UI specifics for MapEntries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/addEntryMapContextMenuAction.hpp>

void babelwires::MapEntryModel::init() {}

QVariant babelwires::MapEntryModel::getDisplayData(unsigned int column) const {
    return {};
}

void babelwires::MapEntryModel::getContextMenuActions(std::vector<std::unique_ptr<ContextMenuAction>>& actionsOut) const {
    actionsOut.emplace_back(std::make_unique<AddEntryMapContextMenuAction>("Add entry above", m_row));
}
