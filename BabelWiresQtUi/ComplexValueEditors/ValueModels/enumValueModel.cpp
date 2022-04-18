/**
 * Model for IntValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/enumValueModel.hpp>

#include "BabelWiresQtUi/ValueEditors/dropDownValueEditor.hpp"

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

QVariant babelwires::EnumValueModel::getDisplayData() const {
    //const Enum* e = m_type->as<Enum>();
    //assert(e && "Expecting an enum here");

    const EnumValue* v = getValue()->as<EnumValue>();
    assert(v && "Expecting an enum here");

    const Identifier value = v->get();
    return QString(IdentifierRegistry::read()->getName(value).c_str());    
}

QWidget* babelwires::EnumValueModel::createEditor(const QModelIndex& index, QWidget* parent) const {
    const Enum *const e = m_type->as<Enum>();
    auto dropDownBox = std::make_unique<DropDownValueEditor>(parent, index);
    {
        IdentifierRegistry::ReadAccess identifierRegistry = IdentifierRegistry::read();
        for (auto enumValue : e->getEnumValues()) {
            dropDownBox->addItem(identifierRegistry->getName(enumValue).c_str());
        }
    }
    return dropDownBox.release();
}

void babelwires::EnumValueModel::setEditorData(QWidget* editor) const {
    const EnumValue* enumValue = m_value->as<EnumValue>();
    const Identifier value = enumValue->get();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const Enum *const e = m_type->as<Enum>();
    unsigned int currentIndex = e->getIndexFromIdentifier(value);
    dropDownBox->setCurrentIndex(currentIndex);
}

std::unique_ptr<babelwires::Value> babelwires::EnumValueModel::getValueFromEditor(QWidget* editor) const {
    const Enum *const e = m_type->as<Enum>();
    const babelwires::Enum::EnumValues& values = e->getEnumValues();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const int newIndex = dropDownBox->currentIndex();
    assert(newIndex >= 0);
    assert(newIndex < values.size());
    const Identifier newValue = values[newIndex];
    auto newEnumValue = std::make_unique<babelwires::EnumValue>();
    newEnumValue->set(newValue);
    return newEnumValue;
}
