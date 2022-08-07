/**
 * The row model for EnumFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/enumRowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ValueEditors/dropDownValueEditor.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Features/enumFeature.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <QString>

#include <cassert>

const babelwires::EnumFeature& babelwires::EnumRowModel::getEnumFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::EnumFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::EnumFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::EnumRowModel::getValueDisplayData() const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    const Identifier value = enumFeature.get();
    return QString(IdentifierRegistry::read()->getName(value).c_str());
}

QWidget* babelwires::EnumRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    auto dropDownBox = std::make_unique<DropDownValueEditor>(parent, index);
    {
        IdentifierRegistry::ReadAccess identifierRegistry = IdentifierRegistry::read();
        for (auto enumValue : enumFeature.getEnum().getEnumValues()) {
            dropDownBox->addItem(identifierRegistry->getName(enumValue).c_str());
        }
    }
    return dropDownBox.release();
}

void babelwires::EnumRowModel::setEditorData(QWidget* editor) const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    const Identifier value = enumFeature.get();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    unsigned int currentIndex = enumFeature.getEnum().getIndexFromIdentifier(value);
    dropDownBox->setCurrentIndex(currentIndex);
}

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::EnumRowModel::createCommandFromEditor(QWidget* editor) const {
    const babelwires::EnumFeature& enumFeature = getEnumFeature();
    const babelwires::Enum::EnumValues& values = enumFeature.getEnum().getEnumValues();
    const Identifier value = enumFeature.get();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const int newIndex = dropDownBox->currentIndex();
    assert(newIndex >= 0);
    assert(newIndex < values.size());
    const Identifier newValue = values[newIndex];
    if (value != newValue) {
        auto modifier = std::make_unique<babelwires::EnumValueAssignmentData>();
        modifier->m_pathToFeature = babelwires::FeaturePath(&enumFeature);
        modifier->m_value = newValue;
        return std::make_unique<AddModifierCommand>("Set enum value", m_featureElement->getElementId(),
                                                    std::move(modifier));
    } else {
        return nullptr;
    }
    return nullptr;
}

bool babelwires::EnumRowModel::isItemEditable() const {
    return getInputFeature();
}
