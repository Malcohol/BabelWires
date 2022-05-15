/**
 * The DiscreteMapEntryModel provides the specific UI for a MapEntry with DiscreteMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/discreteMapEntryModel.hpp>

#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

const babelwires::DiscreteMapEntryData& babelwires::DiscreteMapEntryModel::getDiscreteMapEntryData() const {
    assert(m_mapProjectEntry);
    assert(m_mapProjectEntry->getData().as<DiscreteMapEntryData>());
    return static_cast<const babelwires::DiscreteMapEntryData&>(m_mapProjectEntry->getData());
}

void babelwires::DiscreteMapEntryModel::init() {
    const DiscreteMapEntryData& discreteMapEntry = getDiscreteMapEntryData();
    m_sourceValueModel.init(*m_sourceType, *discreteMapEntry.getSourceValue());
    m_targetValueModel.init(*m_targetType, *discreteMapEntry.getTargetValue());
}

QVariant babelwires::DiscreteMapEntryModel::getDisplayData(unsigned int column) const {
    switch (column) {
        case 0:
            return m_sourceValueModel->getDisplayData();
        case 1:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
    }
}

bool babelwires::DiscreteMapEntryModel::isItemEditable(unsigned int column) const {
    switch (column) {
        case 0:
            return m_sourceValueModel->isItemEditable();
        case 1:
            return m_targetValueModel->isItemEditable();
        default:
            assert(false);
    }
}

QWidget* babelwires::DiscreteMapEntryModel::createEditor(const QModelIndex& index, QWidget* parent) const {
    unsigned int column = static_cast<unsigned int>(index.column());
    switch (column) {
        case 0:
            return m_sourceValueModel->createEditor(index, parent);
        case 1:
            return m_targetValueModel->createEditor(index, parent);
        default:
            assert(false && "That column isn't editable");
    }
}

void babelwires::DiscreteMapEntryModel::setEditorData(unsigned int column, QWidget* editor) const {
    switch (column) {
        case 0:
            return m_sourceValueModel->setEditorData(editor);
        case 1:
            return m_targetValueModel->setEditorData(editor);
        default:
            assert(false && "That column isn't editable");
    }
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::DiscreteMapEntryModel::createReplacementDataFromEditor(unsigned int column, QWidget* editor) const {
    switch (column) {
        case 0:
            if (std::unique_ptr<Value> newValue = m_sourceValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::DiscreteMapEntryData* currentDiscreteData = currentData->as<DiscreteMapEntryData>();
                currentDiscreteData->setSourceValue(std::move(newValue));
                return currentData;
            }
            break;
        case 1:
            if (std::unique_ptr<Value> newValue = m_targetValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::DiscreteMapEntryData* currentDiscreteData = currentData->as<DiscreteMapEntryData>();
                currentDiscreteData->setTargetValue(std::move(newValue));
                return currentData;
            }
            break;
        default:
            assert(false && "That column isn't editable");
    }
    return {};
}

bool babelwires::DiscreteMapEntryModel::validateEditor(QWidget* editor, unsigned int column) const {
    assert(isItemEditable(column) && "That column isn't editable");
    switch (column) {
        case 0:
            return m_sourceValueModel->validateEditor(editor);
        case 1:
            return m_targetValueModel->validateEditor(editor);
        default:
            assert(false && "That column isn't editable");
    }
}
