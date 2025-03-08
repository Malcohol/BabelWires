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

QVariant babelwires::MapEntryModel::getDisplayData() const {
    return {};
}

void babelwires::MapEntryModel::getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut) const {
    auto entryGroup = std::make_unique<ContextMenuGroup>("Map entries");
    auto addEntryAbove = std::make_unique<AddEntryMapContextMenuAction>("Add entry above", m_row);
    auto addEntryBelow = std::make_unique<AddEntryMapContextMenuAction>("Add entry below", m_row + 1);
    auto removeEntry = std::make_unique<RemoveEntryMapContextMenuAction>("Remove entry", m_row);

    if (m_isLastRow) {
        addEntryBelow->setDisabled(true);
        removeEntry->setDisabled(true);
    }
    const MapEntryData& data = m_mapProjectEntry->getData();
    const MapEntryData::Kind currentKind = data.getKind();

    auto resetEntry =
        std::make_unique<ChangeEntryKindContextMenuAction>("Reset entry", currentKind, m_row, false);
    if ((m_sourceType == nullptr) || (m_targetType == nullptr)) {
        addEntryAbove->setDisabled(true);
        addEntryBelow->setDisabled(true);
        resetEntry->setDisabled(true);
    }

    entryGroup->addContextMenuAction(std::move(addEntryAbove));
    entryGroup->addContextMenuAction(std::move(addEntryBelow));
    entryGroup->addContextMenuAction(std::move(removeEntry));
    entryGroup->addContextMenuAction(std::move(resetEntry));

    auto entryTypeGroup = std::make_unique<ContextMenuGroup>("Map entry type");

    for (int i = 0; i < static_cast<int>(MapEntryData::Kind::NUM_VALUES); ++i) {
        const MapEntryData::Kind kind = static_cast<MapEntryData::Kind>(i);
        const auto actionName = QString(MapEntryData::getKindName(kind).c_str());
        auto action =
            std::make_unique<ChangeEntryKindContextMenuAction>(actionName, kind, m_row, true);
        if (kind == currentKind) {
            action->setChecked(true);
        }
        if ((m_isLastRow != MapEntryData::isFallback(kind)) || (m_sourceType == nullptr) || (m_targetType == nullptr)) {
            action->setDisabled(true);
        }
        entryTypeGroup->addContextMenuAction(std::move(action));
    }
    actionsOut.emplace_back(std::move(entryGroup));
    actionsOut.emplace_back(std::move(entryTypeGroup));
}

bool babelwires::MapEntryModel::isItemEditable() const {
    return false;
}

QWidget* babelwires::MapEntryModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    assert(false && "This item isn't editable");
    return nullptr;
}

void babelwires::MapEntryModel::setEditorData(QWidget* editor) const {
    assert(false && "This item isn't editable");
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::MapEntryModel::createReplacementDataFromEditor(QWidget* editor) const {
    assert(false && "This item isn't editable");
    return {};
}

bool babelwires::MapEntryModel::validateEditor(QWidget* editor) const {
    return false;
}
