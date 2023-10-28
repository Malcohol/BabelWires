/**
 * The AllToOneFallbackMapEntryModel provides the specific UI for a MapEntry with AllToOneFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

const babelwires::AllToOneFallbackMapEntryData&
babelwires::AllToOneFallbackMapEntryModel::getAllToOneFallbackMapEntryData() const {
    assert(m_mapProjectEntry);
    assert(m_mapProjectEntry->getData().as<AllToOneFallbackMapEntryData>());
    return static_cast<const babelwires::AllToOneFallbackMapEntryData&>(m_mapProjectEntry->getData());
}

void babelwires::AllToOneFallbackMapEntryModel::init(const ValueModelRegistry& valueModelRegistry) {
    const AllToOneFallbackMapEntryData& allToOneFallbackMapEntry = getAllToOneFallbackMapEntryData();
    m_targetValueModel.init(valueModelRegistry, *m_targetType, *allToOneFallbackMapEntry.getTargetValue(), false, false);
}

QVariant babelwires::AllToOneFallbackMapEntryModel::getDisplayData(Column column) const {
    switch (column) {
        case Column::sourceValue:
            return "*";
        case Column::targetValue:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
            return {};
    }
}

bool babelwires::AllToOneFallbackMapEntryModel::isItemEditable(Column column) const {
    return (column == Column::targetValue) && m_targetValueModel->isItemEditable();
}

QWidget* babelwires::AllToOneFallbackMapEntryModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const Column column = indexToColumn(index);
    assert(isItemEditable(column) && "That column isn't editable");
    return m_targetValueModel->createEditor(parent, index);
}

void babelwires::AllToOneFallbackMapEntryModel::setEditorData(Column column, QWidget* editor) const {
    assert(isItemEditable(column) && "That column isn't editable");
    m_targetValueModel->setEditorData(editor);
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::AllToOneFallbackMapEntryModel::createReplacementDataFromEditor(Column column, QWidget* editor) const {
    assert(isItemEditable(column) && "That column isn't editable");

    if (EditableValueHolder newValue = m_targetValueModel->createValueFromEditorIfDifferent(editor)) {
        std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
        babelwires::AllToOneFallbackMapEntryData *const currentAllToOneData = currentData->as<AllToOneFallbackMapEntryData>();
        assert(currentAllToOneData && "Unexpected MapEntryData");
        currentAllToOneData->setTargetValue(std::move(newValue));
        return currentData;
    }
    return {};
}

bool babelwires::AllToOneFallbackMapEntryModel::validateEditor(QWidget* editor, Column column) const {
    assert(isItemEditable(column) && "That column isn't editable");
    return m_targetValueModel->validateEditor(editor);
}
