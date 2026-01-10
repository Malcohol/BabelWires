/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::Type::Type(TypeExp&& typeExpOfThis)
    : m_typeExp(std::move(typeExpOfThis)) {}

babelwires::Type::~Type() = default;

std::string babelwires::Type::getName() const {
    return getTypeExp().toString();
}

const babelwires::TypeExp& babelwires::Type::getTypeExp() const {
    return m_typeExp;
}

std::optional<babelwires::SubtypeOrder> babelwires::Type::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                               const Type& other) const {
    return {};
}

void babelwires::Type::addTag(Tag tag) {
    m_tags.emplace_back(tag);
}

const std::vector<babelwires::Type::Tag>& babelwires::Type::getTags() const {
    return m_tags;
}

bool babelwires::Type::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    ChildValueVisitor visitor = [&](const TypeSystem& typeSystem, const TypePtr& childType, const Value& childValue,
                                    const PathStep& stepToChild) {
        return childType->isValidValue(typeSystem, childValue);
    };
    return visitValue(typeSystem, v, visitor);
}
