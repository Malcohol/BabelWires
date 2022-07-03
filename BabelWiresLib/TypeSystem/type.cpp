/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::Type::Type(LongIdentifier identifier, VersionNumber version, std::optional<LongIdentifier> parentTypeId)
    : RegistryEntry(identifier, version)
    , m_parentTypeId(parentTypeId)
{}

bool babelwires::Type::verifyParent(const Type& parentType) const {
    assert(false);
    return false;
}

std::optional<babelwires::LongIdentifier> babelwires::Type::getParentTypeId() const {
    return m_parentTypeId;
}

void babelwires::Type::addChild(const Type* child) {
    assert(child);
    m_children.emplace_back(child);
}

const std::vector<const babelwires::Type*>& babelwires::Type::getChildren() const {
    return m_children;
}

void babelwires::Type::setParent(const Type* parent) {
    assert(parent);
    assert(m_parentTypeId);
    assert(parent->getIdentifier() == *m_parentTypeId);
    verifyParent(*parent);
    m_parent = parent;
}

const babelwires::Type* babelwires::Type::getParent() const {
    return m_parent;
}

bool babelwires::Type::isSubType(const Type& supertype) const {
    const Type* current = this;
    while (current && (current != &supertype)) {
        current = current->getParent();
    }
    return current;
}
