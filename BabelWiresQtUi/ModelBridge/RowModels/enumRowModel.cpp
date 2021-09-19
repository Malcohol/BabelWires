/**
 * The row model for EnumFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/RowModels/enumRowModel.hpp"

#include "BabelWiresLib/Enums/enum.hpp"
#include "BabelWiresLib/Features/enumFeature.hpp"
#include "BabelWiresLib/Features/Path/fieldNameRegistry.hpp"

#include "BabelWiresQtUi/ModelBridge/ValueEditors/dropDownValueEditor.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include <QString>

#include <cassert>

const babelwires::EnumFeature& babelwires::EnumRowModel::getEnumFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::EnumFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::EnumFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::EnumRowModel::getValueDisplayData() const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    const FieldIdentifier value = enumFeature.get();
    return QString(FieldNameRegistry::read()->getFieldName(value).c_str());
}

QWidget* babelwires::EnumRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    auto dropDownBox = std::make_unique<DropDownValueEditor>(parent, index);
    {
        FieldNameRegistry::ReadAccess fieldNameRegistry = FieldNameRegistry::read();
        for (auto enumValue : enumFeature.getEnum().getEnumValues()) {
            dropDownBox->addItem(fieldNameRegistry->getFieldName(enumValue).c_str());
        }
    }
    return dropDownBox.release();
}

void babelwires::EnumRowModel::setEditorData(QWidget* editor) const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    const FieldIdentifier value = enumFeature.get();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    unsigned int currentIndex = enumFeature.getEnum().getIndexFromIdentifier(value);
    dropDownBox->setCurrentIndex(currentIndex);
}

std::unique_ptr<babelwires::ModifierData> babelwires::EnumRowModel::createModifierFromEditor(QWidget* editor) const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    const babelwires::Enum::EnumValues& values = enumFeature.getEnum().getEnumValues();
    const FieldIdentifier value = enumFeature.get();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const int newIndex = dropDownBox->currentIndex();
    assert(newIndex >= 0);
    assert(newIndex < values.size());
    const FieldIdentifier newValue = values[newIndex];
    if (value != newValue) {
        auto modifier = std::make_unique<babelwires::EnumValueAssignmentData>();
        modifier->m_pathToFeature = babelwires::FeaturePath(&enumFeature);
        modifier->m_value = newValue;
        return modifier;
    } else {
        return nullptr;
    }
   return nullptr;
}

bool babelwires::EnumRowModel::isItemEditable() const {
    return getInputFeature();
}
