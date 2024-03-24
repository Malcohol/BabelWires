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

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

void babelwires::MapEntryModel::init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem) {}

QVariant babelwires::MapEntryModel::getDisplayData(Column column) const {
    return {};
}

void babelwires::MapEntryModel::getContextMenuActions(
    std::vector<std::unique_ptr<ContextMenuAction>>& actionsOut) const {
    actionsOut.reserve(8);
    auto& addEntryAbove =
        actionsOut.emplace_back(std::make_unique<AddEntryMapContextMenuAction>("Add entry above", m_row));
    auto& addEntryBelow =
        actionsOut.emplace_back(std::make_unique<AddEntryMapContextMenuAction>("Add entry below", m_row + 1));
    auto& removeEntry =
        actionsOut.emplace_back(std::make_unique<RemoveEntryMapContextMenuAction>("Remove entry", m_row));

    if (m_isLastRow) {
        addEntryBelow->setDisabled(true);
        removeEntry->setDisabled(true);
    }
    const MapEntryData& data = m_mapProjectEntry->getData();
    const MapEntryData::Kind currentKind = data.getKind();

    auto& resetEntry = actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>("Reset entry", currentKind, m_row));
    if ((m_sourceType == nullptr) || (m_targetType == nullptr)) {
        addEntryAbove->setDisabled(true);
        addEntryBelow->setDisabled(true);
        resetEntry->setDisabled(true);
    }

    for (int i = 0; i < static_cast<int>(MapEntryData::Kind::NUM_VALUES); ++i) {
        const MapEntryData::Kind kind = static_cast<MapEntryData::Kind>(i);
        const auto actionName = QString("Change entry type to \"%1\"").arg(MapEntryData::getKindName(kind).c_str());
        auto& action =
            actionsOut.emplace_back(std::make_unique<ChangeEntryKindContextMenuAction>(actionName, kind, m_row));
        if ((kind == currentKind) || (m_isLastRow != MapEntryData::isFallback(kind)) || (m_sourceType == nullptr) ||
            (m_targetType == nullptr)) {
            action->setDisabled(true);
        }
    }
}

bool babelwires::MapEntryModel::isItemEditable(Column column) const {
    return false;
}

QWidget* babelwires::MapEntryModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    assert(false && "This item isn't editable");
    return nullptr;
}

void babelwires::MapEntryModel::setEditorData(Column column, QWidget* editor) const {
    assert(false && "This item isn't editable");
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::MapEntryModel::createReplacementDataFromEditor(Column column, QWidget* editor) const {
    assert(false && "This item isn't editable");
    return {};
}

bool babelwires::MapEntryModel::validateEditor(QWidget* editor, Column column) const {
    return false;
}

babelwires::MapEntryModel::Column babelwires::MapEntryModel::indexToColumn(const QModelIndex& index) {
    assert(index.column() >= 0);
    assert(index.column() < 2);
    return static_cast<Column>(index.column());
}
