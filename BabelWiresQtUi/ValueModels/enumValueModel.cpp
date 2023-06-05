/**
 * Model for EnumValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/enumValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/dropDownValueEditor.hpp>

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

QWidget* babelwires::EnumValueModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const EnumType* const e = m_type->as<EnumType>();
    auto dropDownBox = std::make_unique<DropDownValueEditor>(parent, index);
    {
        IdentifierRegistry::ReadAccess identifierRegistry = IdentifierRegistry::read();
        for (auto enumValue : e->getValueSet()) {
            dropDownBox->addItem(identifierRegistry->getName(enumValue).c_str());
        }
    }
    return dropDownBox.release();
}

void babelwires::EnumValueModel::setEditorData(QWidget* editor) const {
    const EnumValue* enumValue = m_value->as<EnumValue>();
    const ShortId value = enumValue->get();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const EnumType* const e = m_type->as<EnumType>();
    unsigned int currentIndex = e->getIndexFromIdentifier(value);
    dropDownBox->setCurrentIndex(currentIndex);
}

babelwires::EditableValueHolder babelwires::EnumValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    const EnumType* const e = m_type->as<EnumType>();
    const babelwires::EnumType::ValueSet& values = e->getValueSet();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const int newIndex = dropDownBox->currentIndex();
    assert(newIndex >= 0);
    assert(newIndex < values.size());
    const ShortId newValue = values[newIndex];

    const EnumValue* const enumValue = m_value->as<EnumValue>();
    assert(enumValue && "Expecting an enum value here");
    const ShortId currentValue = enumValue->get();

    if (newValue != currentValue) {
        return EditableValueHolder::makeValue<babelwires::EnumValue>(newValue);
    }
    return {};
}

bool babelwires::EnumValueModel::isItemEditable() const {
    return m_value->as<EnumValue>();
}

bool babelwires::EnumValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<DropDownValueEditor*>(editor);
}
