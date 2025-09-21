/**
 * A dummy value constructed by TypeVariableType.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/typeVariableValue.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>

std::size_t babelwires::TypeVariableValue::getHash() const {
    // TODO: Use a compile-time hash.
    return std::hash<const char*>()("Unassigned");
}

bool babelwires::TypeVariableValue::operator==(const Value& other) const {
    return other.as<TypeVariableValue>();
}
