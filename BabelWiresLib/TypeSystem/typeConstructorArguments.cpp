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

babelwires::TypeConstructorArgumentsOld::~TypeConstructorArgumentsOld() = default;

std::size_t babelwires::TypeConstructorArgumentsOld::getHash() const {
    std::size_t hash = 0x80235AA2;
    for (const auto& arg : m_typeArguments) {
        hash::mixInto(hash, arg);
    }
    return hash;
}
