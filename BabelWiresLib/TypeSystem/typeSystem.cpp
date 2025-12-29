/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>

#include <algorithm>
namespace {
    void insertTypeId(babelwires::TypeSystem::TypeIdSet& typeIds, const babelwires::RegisteredTypeId& typeId) {
        auto it = std::upper_bound(typeIds.begin(), typeIds.end(), typeId);
        typeIds.insert(it, typeId);
    }
} // namespace

babelwires::TypeSystem::TypeSystem() = default;
babelwires::TypeSystem::~TypeSystem() = default;

const babelwires::Type* babelwires::TypeSystem::tryGetRegisteredType(RegisteredTypeId id) const {
    auto it = m_registeredTypeRegistry.find(id);
    if (it != m_registeredTypeRegistry.end()) {
        return std::get<0>(it->second).get();
    }
    return nullptr;
}

const babelwires::Type& babelwires::TypeSystem::getRegisteredType(RegisteredTypeId id) const {
    auto it = m_registeredTypeRegistry.find(id);
    assert((it != m_registeredTypeRegistry.end()) && "Primitive Type not registered in type system");
    return *std::get<0>(it->second);
}

babelwires::Type* babelwires::TypeSystem::addRegisteredType(LongId typeId, VersionNumber version,
                                                           TypePtr newType) {
    auto addResult = m_registeredTypeRegistry.emplace(
        std::pair<LongId, RegisteredTypeInfo>{typeId, RegisteredTypeInfo{std::move(newType), version}});
    assert(addResult.second && "Type with that identifier already registered");
    babelwires::Type* const newTypeRaw = std::get<0>(addResult.first->second).get();
    for (auto it : newTypeRaw->getTags()) {
        m_taggedRegisteredTypes[it].emplace_back(typeId);
    }
    return newTypeRaw;
}

const babelwires::TypeConstructor* babelwires::TypeSystem::tryGetTypeConstructor(TypeConstructorId id) const {
    auto it = m_typeConstructorRegistry.find(id);
    if (it != m_typeConstructorRegistry.end()) {
        return std::get<0>(it->second).get();
    }
    return nullptr;
}

const babelwires::TypeConstructor& babelwires::TypeSystem::getTypeConstructor(TypeConstructorId id) const {
    auto it = m_typeConstructorRegistry.find(id);
    assert((it != m_typeConstructorRegistry.end()) && "TypeConstructor not registered in type system");
    return *std::get<0>(it->second);
}

babelwires::TypeConstructor*
babelwires::TypeSystem::addTypeConstructorInternal(TypeConstructorId typeId, VersionNumber version,
                                                   std::unique_ptr<TypeConstructor> newTypeConstructor) {
    auto addResult = m_typeConstructorRegistry.emplace(std::pair<TypeConstructorId, TypeConstructorInfo>{
        typeId, TypeConstructorInfo{std::move(newTypeConstructor), version}});
    assert(addResult.second && "TypeConstructor with that identifier already registered");
    return std::get<0>(addResult.first->second).get();
}

babelwires::SubtypeOrder babelwires::TypeSystem::compareSubtype(const TypeRef& typeRefA,
                                                                const TypeRef& typeRefB) const {
    // TODO Thread-safe cache here.
    if (typeRefA == typeRefB) {
        return SubtypeOrder::IsEquivalent;
    }
    if (const Type* typeA = typeRefA.tryResolve(*this)) {
        if (const Type* typeB = typeRefB.tryResolve(*this)) {
            if (const auto resultFromA = typeA->compareSubtypeHelper(*this, *typeB)) {
                return *resultFromA;
            } else if (const auto resultFromB = typeB->compareSubtypeHelper(*this, *typeA)) {
                return reverseSubtypeOrder(*resultFromB);
            }
        }
    }
    return SubtypeOrder::IsDisjoint;
}

bool babelwires::TypeSystem::isSubType(const TypeRef& typeRefA, const TypeRef& typeRefB) const {
    SubtypeOrder order = compareSubtype(typeRefA, typeRefB);
    return (order == SubtypeOrder::IsEquivalent) || (order == SubtypeOrder::IsSubtype);
}

bool babelwires::TypeSystem::isRelatedType(const TypeRef& typeRefA, const TypeRef& typeRefB) const {
    return compareSubtype(typeRefA, typeRefB) != SubtypeOrder::IsDisjoint;
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getTaggedRegisteredTypes(Type::Tag tag) const {
    const auto it = m_taggedRegisteredTypes.find(tag);
    if (it != m_taggedRegisteredTypes.end()) {
        return it->second;
    }
    return {};
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllRegisteredTypes() const {
    babelwires::TypeSystem::TypeIdSet result;
    for (const auto& it : m_registeredTypeRegistry) {
        result.emplace_back(it.first);
    }
    return result;
}
