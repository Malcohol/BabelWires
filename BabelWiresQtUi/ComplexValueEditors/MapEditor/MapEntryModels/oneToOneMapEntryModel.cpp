/**
 * The OneToOneMapEntryModel provides the specific UI for a MapEntry with OneToOneMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/oneToOneMapEntryModel.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>

const babelwires::OneToOneMapEntryData& babelwires::OneToOneMapEntryModel::getOneToOneMapEntryData() const {
    assert(m_mapProjectEntry);
    assert(m_mapProjectEntry->getData().tryAs<OneToOneMapEntryData>());
    return static_cast<const babelwires::OneToOneMapEntryData&>(m_mapProjectEntry->getData());
}

void babelwires::OneToOneMapEntryModel::init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem) {
    const OneToOneMapEntryData& discreteMapEntry = getOneToOneMapEntryData();
    m_sourceValueModel.init(valueModelRegistry, typeSystem, m_sourceType, discreteMapEntry.getSourceValue(), false, false);
    m_targetValueModel.init(valueModelRegistry, typeSystem, m_targetType, discreteMapEntry.getTargetValue(), false, false);
}

QVariant babelwires::OneToOneMapEntryModel::getDisplayData() const {
    switch (m_column) {
        case Column::sourceValue:
            return m_sourceValueModel->getDisplayData();
        case Column::targetValue:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false && "Not a valid column");
            return {};
    }
}

bool babelwires::OneToOneMapEntryModel::isItemEditable() const {
    switch (m_column) {
        case Column::sourceValue:
            return m_sourceValueModel->isItemEditable();
        case Column::targetValue:
            return m_targetValueModel->isItemEditable();
        default:
            assert(false && "Not a valid column");
            return false;
    }
}

QWidget* babelwires::OneToOneMapEntryModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    switch (m_column) {
        case Column::sourceValue:
            return m_sourceValueModel->createEditor(parent);
        case Column::targetValue:
            return m_targetValueModel->createEditor(parent);
        default:
            assert(false && "Not a valid column");
            return nullptr;
    }
}

void babelwires::OneToOneMapEntryModel::setEditorData(QWidget* editor) const {
    switch (m_column) {
        case Column::sourceValue:
            return m_sourceValueModel->setEditorData(editor);
        case Column::targetValue:
            return m_targetValueModel->setEditorData(editor);
        default:
            assert(false && "Not a valid column");
    }
}

std::unique_ptr<babelwires::MapEntryData>
babelwires::OneToOneMapEntryModel::createReplacementDataFromEditor(QWidget* editor) const {
    switch (m_column) {
        case Column::sourceValue:
            if (ValueHolder newValue = m_sourceValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::OneToOneMapEntryData* currentDiscreteData = currentData->tryAs<OneToOneMapEntryData>();
                currentDiscreteData->setSourceValue(std::move(newValue));
                return currentData;
            }
            break;
        case Column::targetValue:
            if (ValueHolder newValue = m_targetValueModel->createValueFromEditorIfDifferent(editor)) {
                std::unique_ptr<babelwires::MapEntryData> currentData = m_mapProjectEntry->getData().clone();
                babelwires::OneToOneMapEntryData* currentDiscreteData = currentData->tryAs<OneToOneMapEntryData>();
                currentDiscreteData->setTargetValue(std::move(newValue));
                return currentData;
            }
            break;
        default:
            assert(false && "Not a valid column");
    }
    return {};
}

bool babelwires::OneToOneMapEntryModel::validateEditor(QWidget* editor) const {
    assert(isItemEditable() && "This column isn't editable");
    switch (m_column) {
        case Column::sourceValue:
            return m_sourceValueModel->validateEditor(editor);
        case Column::targetValue:
            return m_targetValueModel->validateEditor(editor);
        default:
            assert(false && "Not a valid column");
            return false;
    }
}

void babelwires::OneToOneMapEntryModel::getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut) const {
    switch(m_column) {
        case Column::sourceValue:{
            MapProjectDataLocation sourceDataLocation(m_row, MapProjectDataLocation::Side::source, Path());
            m_sourceValueModel->getContextMenuActions(sourceDataLocation, actionsOut);
            break;
        }
        case Column::targetValue: {
            MapProjectDataLocation targetDataLocation(m_row, MapProjectDataLocation::Side::target, Path());
            m_targetValueModel->getContextMenuActions(targetDataLocation, actionsOut);
            break;
        }
        default:
            assert(false && "Not a valid column");
    }
    MapEntryModel::getContextMenuActions(actionsOut);
}