/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::Type::~Type() = default;

bool babelwires::Type::verifySupertype(const Type& supertype) const {
    assert(false);
    return false;
}

bool babelwires::Type::isAbstract() const {
    return false;
}

std::string babelwires::Type::getName() const {
    return getTypeRef().toString();
}

babelwires::SubtypeOrder babelwires::Type::compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const {
    return SubtypeOrder::IsUnrelated;
}

void babelwires::Type::addTag(Tag tag) {
    m_tags.emplace_back(tag);
}

const std::vector<babelwires::Type::Tag>& babelwires::Type::getTags() const { 
    return m_tags;
}
