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
    return true;
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

std::unique_ptr<babelwires::MapEntryData> babelwires::DiscreteMapEntryModel::createReplacementDataFromEditor(unsigned int column, QWidget* editor) const {
     std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
     babelwires::DiscreteMapEntryData* currentDiscreteData = currentData->as<DiscreteMapEntryData>();
     switch (column) {
        case 0:
            currentDiscreteData->setSourceValue(m_sourceValueModel->getValueFromEditor(editor));
            break;
        case 1:
            currentDiscreteData->setTargetValue(m_targetValueModel->getValueFromEditor(editor));
            break;
        default:
            assert(false && "That column isn't editable");
    }
    return currentData;
}
