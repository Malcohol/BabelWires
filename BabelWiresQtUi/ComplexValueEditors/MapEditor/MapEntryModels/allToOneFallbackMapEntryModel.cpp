/**
 * The AllToOneFallbackMapEntryModel provides the specific UI for a MapEntry with AllToOneFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

const babelwires::AllToOneFallbackMapEntryData&
babelwires::AllToOneFallbackMapEntryModel::getAllToOneFallbackMapEntryData() const {
    assert(m_mapProjectEntry);
    assert(m_mapProjectEntry->getData().as<AllToOneFallbackMapEntryData>());
    return static_cast<const babelwires::AllToOneFallbackMapEntryData&>(m_mapProjectEntry->getData());
}

void babelwires::AllToOneFallbackMapEntryModel::init() {
    const AllToOneFallbackMapEntryData& allToOneFallbackMapEntry = getAllToOneFallbackMapEntryData();
    m_targetValueModel.init(*m_targetType, *allToOneFallbackMapEntry.getTargetValue());
}

QVariant babelwires::AllToOneFallbackMapEntryModel::getDisplayData(unsigned int column) const {
    switch (column) {
        case 0:
            return "*";
        case 1:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
    }
}

bool babelwires::AllToOneFallbackMapEntryModel::isItemEditable(unsigned int column) const {
    return (column == 1) && m_targetValueModel->isItemEditable();
}

QWidget* babelwires::AllToOneFallbackMapEntryModel::createEditor(const QModelIndex& index, QWidget* parent) const {
    unsigned int column = static_cast<unsigned int>(index.column());
    assert(isItemEditable(column) && "That column isn't editable");
    return m_targetValueModel->createEditor(index, parent);
}

void babelwires::AllToOneFallbackMapEntryModel::setEditorData(unsigned int column, QWidget* editor) const {
    assert(isItemEditable(column) && "That column isn't editable");
    m_targetValueModel->setEditorData(editor);
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::AllToOneFallbackMapEntryModel::createReplacementDataFromEditor(unsigned int column, QWidget* editor) const {
    assert(isItemEditable(column) && "That column isn't editable");

    if (std::unique_ptr<Value> newValue = m_targetValueModel->createValueFromEditorIfDifferent(editor)) {
        std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
        babelwires::AllToOneFallbackMapEntryData *const currentAllToOneData = currentData->as<AllToOneFallbackMapEntryData>();
        assert(currentAllToOneData && "Unexpected MapEntryData");
        currentAllToOneData->setTargetValue(std::move(newValue));
        return currentData;
    }
    return {};
}
