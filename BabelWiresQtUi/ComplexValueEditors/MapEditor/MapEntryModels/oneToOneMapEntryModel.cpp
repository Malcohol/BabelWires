/**
 * The OneToOneMapEntryModel provides the specific UI for a MapEntry with OneToOneMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/oneToOneMapEntryModel.hpp>

#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

const babelwires::OneToOneMapEntryData& babelwires::OneToOneMapEntryModel::getOneToOneMapEntryData() const {
    assert(m_mapProjectEntry);
    assert(m_mapProjectEntry->getData().as<OneToOneMapEntryData>());
    return static_cast<const babelwires::OneToOneMapEntryData&>(m_mapProjectEntry->getData());
}

void babelwires::OneToOneMapEntryModel::init() {
    const OneToOneMapEntryData& discreteMapEntry = getOneToOneMapEntryData();
    m_sourceValueModel.init(*m_sourceType, *discreteMapEntry.getSourceValue());
    m_targetValueModel.init(*m_targetType, *discreteMapEntry.getTargetValue());
}

QVariant babelwires::OneToOneMapEntryModel::getDisplayData(Column column) const {
    switch (column) {
        case Column::sourceValue:
            return m_sourceValueModel->getDisplayData();
        case Column::targetValue:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
            return {};
    }
}

bool babelwires::OneToOneMapEntryModel::isItemEditable(Column column) const {
    switch (column) {
        case Column::sourceValue:
            return m_sourceValueModel->isItemEditable();
        case Column::targetValue:
            return m_targetValueModel->isItemEditable();
        default:
            assert(false);
            return false;
    }
}

QWidget* babelwires::OneToOneMapEntryModel::createEditor(const QModelIndex& index, QWidget* parent) const {
    assert(index.column() >= 0);
    assert(index.column() < 2);
    const Column column = static_cast<Column>(index.column());
    switch (column) {
        case Column::sourceValue:
            return m_sourceValueModel->createEditor(index, parent);
        case Column::targetValue:
            return m_targetValueModel->createEditor(index, parent);
        default:
            assert(false && "That column isn't editable");
            return nullptr;
    }
}

void babelwires::OneToOneMapEntryModel::setEditorData(Column column, QWidget* editor) const {
    switch (column) {
        case Column::sourceValue:
            return m_sourceValueModel->setEditorData(editor);
        case Column::targetValue:
            return m_targetValueModel->setEditorData(editor);
        default:
            assert(false && "That column isn't editable");
    }
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::OneToOneMapEntryModel::createReplacementDataFromEditor(Column column, QWidget* editor) const {
    switch (column) {
        case Column::sourceValue:
            if (std::unique_ptr<Value> newValue = m_sourceValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::OneToOneMapEntryData* currentDiscreteData = currentData->as<OneToOneMapEntryData>();
                currentDiscreteData->setSourceValue(std::move(newValue));
                return currentData;
            }
            break;
        case Column::targetValue:
            if (std::unique_ptr<Value> newValue = m_targetValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::OneToOneMapEntryData* currentDiscreteData = currentData->as<OneToOneMapEntryData>();
                currentDiscreteData->setTargetValue(std::move(newValue));
                return currentData;
            }
            break;
        default:
            assert(false && "That column isn't editable");
    }
    return {};
}

bool babelwires::OneToOneMapEntryModel::validateEditor(QWidget* editor, Column column) const {
    assert(isItemEditable(column) && "That column isn't editable");
    switch (column) {
        case Column::sourceValue:
            return m_sourceValueModel->validateEditor(editor);
        case Column::targetValue:
            return m_targetValueModel->validateEditor(editor);
        default:
            assert(false && "That column isn't editable");
            return false;
    }
}
