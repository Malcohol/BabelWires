/**
 * Model for IntValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/enumValueModel.hpp>

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
