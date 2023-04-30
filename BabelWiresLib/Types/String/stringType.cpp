/**
 * The type of strings.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::NewValueHolder babelwires::StringType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<StringValue>();
}

bool babelwires::StringType::isValidValue(const Value& v) const {
    return v.as<StringValue>();
}

std::string babelwires::StringType::getKind() const {
    return StringValue::serializationType;
}
