/**
 * The type of text values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Text/textType.hpp>

#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Identifiers/registeredIdentifier.hpp>

babelwires::TextType::TextType(TypeExp&& typeExpOfThis, size_t maxLength)
    : Type(std::move(typeExpOfThis))
    , m_maxLength(maxLength) {}

babelwires::NewValueHolder babelwires::TextType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<TextValue>();
}

bool babelwires::TextType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    if (const TextValue* const textValue = v.tryAs<TextValue>()) {
        return textValue->get().getTextLength() <= m_maxLength;
    }
    return false;
}

std::string babelwires::TextType::getFlavour() const {
    return TextValue::s_serializationTypeName;
}

std::optional<babelwires::SubtypeOrder> babelwires::TextType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                   const Type& other) const {
    if (other.tryAs<TextType>()) {
        return SubtypeOrder::IsEquivalent;
    } else {
        return {};
    }
}

std::string babelwires::TextType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    return v->as<TextValue>().toString();
}

babelwires::DefaultTextType::DefaultTextType()
    : TextType(getThisIdentifier()) {}