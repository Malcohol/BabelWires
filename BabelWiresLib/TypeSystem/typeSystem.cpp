/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <algorithm>
namespace {
    void insertTypeId(babelwires::TypeSystem::TypeIdSet& typeIds, const babelwires::PrimitiveTypeId& typeId) {
        auto it = std::upper_bound(typeIds.begin(), typeIds.end(), typeId);
        typeIds.insert(it, typeId);
    }
} // namespace

babelwires::TypeSystem::TypeSystem() = default;
babelwires::TypeSystem::~TypeSystem() = default;

const babelwires::Type* babelwires::TypeSystem::tryGetPrimitiveType(PrimitiveTypeId id) const {
    auto it = m_primitiveTypeRegistry.find(id);
    if (it != m_primitiveTypeRegistry.end()) {
        return std::get<0>(it->second).get();
    }
    return nullptr;
}

const babelwires::Type& babelwires::TypeSystem::getPrimitiveType(PrimitiveTypeId id) const {
    auto it = m_primitiveTypeRegistry.find(id);
    assert((it != m_primitiveTypeRegistry.end()) && "Primitive Type not registered in type system");
    return *std::get<0>(it->second);
}

babelwires::Type* babelwires::TypeSystem::addPrimitiveType(LongId typeId, VersionNumber version,
                                                           std::unique_ptr<Type> newType) {
    auto addResult = m_primitiveTypeRegistry.emplace(
        std::pair<LongId, PrimitiveTypeInfo>{typeId, PrimitiveTypeInfo{std::move(newType), version}});
    assert(addResult.second && "Type with that identifier already registered");
    babelwires::Type *const newTypeRaw = std::get<0>(addResult.first->second).get();
    for (auto it : newTypeRaw->getTags()) {
        m_taggedPrimitiveTypes[it].emplace_back(typeId);
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
            return typeA->compareSubtypeHelper(*this, *typeB);
        }
    }
    return SubtypeOrder::IsUnrelated;
}

bool babelwires::TypeSystem::isSubType(const TypeRef& typeRefA, const TypeRef& typeRefB) const {
    SubtypeOrder order = compareSubtype(typeRefA, typeRefB);
    return (order == SubtypeOrder::IsEquivalent) || (order == SubtypeOrder::IsSubtype);
}

bool babelwires::TypeSystem::isRelatedType(const TypeRef& typeRefA, const TypeRef& typeRefB) const {
    return compareSubtype(typeRefA, typeRefB) != SubtypeOrder::IsUnrelated;
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getTaggedPrimitiveTypes(Type::Tag tag) const {
    const auto it = m_taggedPrimitiveTypes.find(tag);
    if (it != m_taggedPrimitiveTypes.end()) {
        return it->second;
    }
    return {};
}
