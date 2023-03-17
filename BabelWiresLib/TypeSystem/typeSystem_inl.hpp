/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

template<unsigned int N>
const babelwires::TypeConstructor<N>* babelwires::TypeSystem::tryGetTypeConstructor(TypeConstructorId id) const {
    auto it = std::get<N-1>(m_typeConstructorRegistry).find(id);
    if (it != std::get<N-1>(m_typeConstructorRegistry).end()) {
        return std::get<0>(it->second).get();
    }
    return nullptr;
}

template<unsigned int N>
const babelwires::TypeConstructor<N>& babelwires::TypeSystem::getTypeConstructor(TypeConstructorId id) const {
    auto it = std::get<N-1>(m_typeConstructorRegistry).find(id);
    assert((it != std::get<N-1>(m_typeConstructorRegistry).end()) && "TypeConstructor not registered in type system");
    return *std::get<0>(it->second);
}

template<unsigned int N>
babelwires::TypeConstructor<N>*
babelwires::TypeSystem::addTypeConstructorInternal(TypeConstructorId typeId, VersionNumber version,
                                                   std::unique_ptr<TypeConstructor<N>> newTypeConstructor) {
    auto addResult = std::get<N-1>(m_typeConstructorRegistry).emplace(std::pair<TypeConstructorId, TypeConstructorInfo<N>>{
        typeId, TypeConstructorInfo<N>{std::move(newTypeConstructor), version}});
    assert(addResult.second && "TypeConstructor with that identifier already registered");
    return std::get<0>(addResult.first->second).get();
}
