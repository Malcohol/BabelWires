/**
 * The type of text values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/String/textType.hpp>

#include <BabelWiresLib/Types/String/textValue.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

#include <BaseLib/Identifiers/registeredIdentifier.hpp>

babelwires::TextType::TextType()
    : Type(getThisIdentifier()) {}

babelwires::NewValueHolder babelwires::TextType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<TextValue>();
}

bool babelwires::TextType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    return v.tryAs<TextValue>();
}

std::string babelwires::TextType::getFlavour() const {
    return TextValue::s_serializationTypeName;
}


std::optional<babelwires::SubtypeOrder> babelwires::TextType::compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const {
    if (other.tryAs<TextType>()) {
        return SubtypeOrder::IsEquivalent;
    } else {
        return {};
    }
}

std::string babelwires::TextType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->as<TextValue>().toString();
}
