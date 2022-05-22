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

QVariant babelwires::OneToOneMapEntryModel::getDisplayData(unsigned int column) const {
    switch (column) {
        case 0:
            return m_sourceValueModel->getDisplayData();
        case 1:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
    }
}

bool babelwires::OneToOneMapEntryModel::isItemEditable(unsigned int column) const {
    switch (column) {
        case 0:
            return m_sourceValueModel->isItemEditable();
        case 1:
            return m_targetValueModel->isItemEditable();
        default:
            assert(false);
    }
}

QWidget* babelwires::OneToOneMapEntryModel::createEditor(const QModelIndex& index, QWidget* parent) const {
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

void babelwires::OneToOneMapEntryModel::setEditorData(unsigned int column, QWidget* editor) const {
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
babelwires::OneToOneMapEntryModel::createReplacementDataFromEditor(unsigned int column, QWidget* editor) const {
    switch (column) {
        case 0:
            if (std::unique_ptr<Value> newValue = m_sourceValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::OneToOneMapEntryData* currentDiscreteData = currentData->as<OneToOneMapEntryData>();
                currentDiscreteData->setSourceValue(std::move(newValue));
                return currentData;
            }
            break;
        case 1:
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

bool babelwires::OneToOneMapEntryModel::validateEditor(QWidget* editor, unsigned int column) const {
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
