/**
 * The type of strings.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

babelwires::StringType::StringType()
    : Type(getThisType()) {}

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::NewValueHolder babelwires::StringType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<StringValue>();
}

bool babelwires::StringType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    return v.as<StringValue>();
}

std::string babelwires::StringType::getFlavour() const {
    return StringValue::serializationType;
}


std::optional<babelwires::SubtypeOrder> babelwires::StringType::compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const {
    if (other.as<StringType>()) {
        return SubtypeOrder::IsEquivalent;
    } else {
        return {};
    }
}

std::string babelwires::StringType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<StringValue>().toString();
}
