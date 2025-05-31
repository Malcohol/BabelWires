/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::Type::~Type() = default;
 
std::string babelwires::Type::getName() const {
    return getTypeRef().toString();
}

std::optional<babelwires::SubtypeOrder> babelwires::Type::compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const {
    return {};
}

void babelwires::Type::addTag(Tag tag) {
    m_tags.emplace_back(tag);
}

const std::vector<babelwires::Type::Tag>& babelwires::Type::getTags() const { 
    return m_tags;
}
