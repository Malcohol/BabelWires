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
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
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

    const MapEntryData& data = m_mapProjectEntry->getData();
    const MapEntryData::Kind currentKind = data.getKind();

    actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>("Reset entry", currentKind, m_row));

    for (int i = 0; i < static_cast<int>(MapEntryData::Kind::NUM_KINDS); ++i) {
        const MapEntryData::Kind kind = static_cast<MapEntryData::Kind>(i);
        const auto actionName = QString("Change entry type to \"%1\"").arg(MapEntryData::getKindName(kind).c_str());
        auto& action = actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>(actionName, kind, m_row));
        if ((kind == currentKind) || (m_isLastRow != MapEntryData::isFallback(kind))) {
            action->setDisabled(true);
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
