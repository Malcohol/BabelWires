/**
 * MapEntryModels provide the UI specifics for MapEntries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/addEntryMapContextMenuAction.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/removeEntryMapContextMenuAction.hpp>

#include <BabelWiresLib/Maps/MapEntries/fallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

void babelwires::MapEntryModel::init() {}

QVariant babelwires::MapEntryModel::getDisplayData(unsigned int column) const {
    return {};
}

void babelwires::MapEntryModel::getContextMenuActions(std::vector<std::unique_ptr<ContextMenuAction>>& actionsOut) const {
    actionsOut.emplace_back(std::make_unique<AddEntryMapContextMenuAction>("Add entry above", m_row));
    auto removeEntry = std::make_unique<RemoveEntryMapContextMenuAction>("Remove entry", m_row);
    if (m_isLastRow && m_mapProjectEntry->getData().as<FallbackMapEntryData>()) {
        removeEntry->setDisabled(true);
    }
    actionsOut.emplace_back(std::move(removeEntry));
}


bool babelwires::MapEntryModel::isItemEditable(unsigned int column) const {
    return false;
}

QWidget* babelwires::MapEntryModel::createEditor(const QModelIndex& index, QWidget* parent) const {
    assert(false && "This item isn't editable");
    return nullptr;
}

void babelwires::MapEntryModel::setEditorData(unsigned int column, QWidget* editor) const {
    assert(false && "This item isn't editable");
}

std::unique_ptr<babelwires::MapEntryData> babelwires::MapEntryModel::createReplacementDataFromEditor(unsigned int column, QWidget* editor) const {
    assert(false && "This item isn't editable");
    return {};    
}
