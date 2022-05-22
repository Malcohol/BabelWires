/**
 * MapEntryModels provide the UI specifics for MapEntries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/addEntryMapContextMenuAction.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/changeEntryKindContextMenuAction.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/removeEntryMapContextMenuAction.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/identityFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

void babelwires::MapEntryModel::init() {}

QVariant babelwires::MapEntryModel::getDisplayData(unsigned int column) const {
    return {};
}

void babelwires::MapEntryModel::getContextMenuActions(
    std::vector<std::unique_ptr<ContextMenuAction>>& actionsOut) const {
    actionsOut.reserve(8);
    actionsOut.emplace_back(std::make_unique<AddEntryMapContextMenuAction>("Add entry above", m_row));
    auto& addEntryBelow =
        actionsOut.emplace_back(std::make_unique<AddEntryMapContextMenuAction>("Add entry below", m_row + 1));
    auto& removeEntry =
        actionsOut.emplace_back(std::make_unique<RemoveEntryMapContextMenuAction>("Remove entry", m_row));
    auto& oneToOne = actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>(
        "Change entry type to \"One to One\"", MapEntryData::Kind::DiscreteEntry, m_row));
    auto& allToOne = actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>(
        "Change entry type to \"All to One\"", MapEntryData::Kind::Fallback_AllToOne, m_row));
    auto& allToSame = actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>(
        "Change entry type to \"All to Same\"", MapEntryData::Kind::Fallback_AllToSame, m_row));

    const MapEntryData& data = m_mapProjectEntry->getData();
    if (m_isLastRow) {
        oneToOne->setDisabled(true);
        if (data.as<FallbackMapEntryData>()) {
            addEntryBelow->setDisabled(true);
            removeEntry->setDisabled(true);
            if (data.as<IdentityFallbackMapEntryData>()) {
                allToSame->setDisabled(true);
            } else if (data.as<AllToOneFallbackMapEntryData>()) {
                allToOne->setDisabled(true);
            }
        }
    } else {
        allToOne->setDisabled(true);
        allToSame->setDisabled(true);
        if (data.as<DiscreteMapEntryData>()) {
            oneToOne->setDisabled(true);
        }
    }
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

std::unique_ptr<babelwires::MapEntryData>
babelwires::MapEntryModel::createReplacementDataFromEditor(unsigned int column, QWidget* editor) const {
    assert(false && "This item isn't editable");
    return {};
}

bool babelwires::MapEntryModel::validateEditor(QWidget* editor, unsigned int column) const {
    return false;
}
