/**
 * A TypeRef identifies a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeConstructorArguments.hpp>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Hash/hash.hpp>

babelwires::TypeConstructorArguments::~TypeConstructorArguments() = default;

std::size_t babelwires::TypeConstructorArguments::getHash() const {
    // Arbitrary value.
    std::size_t hash = 0x80235AA2;
    for (const auto& targ : m_typeArguments) {
        hash::mixInto(hash, targ);
    }
    for (const auto& varg : m_valueArguments) {
        hash::mixInto(hash, varg);
    }
    return hash;
}

bool babelwires::TypeConstructorArguments::equals(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
    return (a.m_typeArguments == b.m_typeArguments) && (a.m_valueArguments == b.m_valueArguments);
}
