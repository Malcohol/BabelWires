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

}
