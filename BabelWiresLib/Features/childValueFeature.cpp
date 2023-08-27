/**
 * A ChildValueFeature is a ValueFeature whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/childValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

babelwires::ChildValueFeature::ChildValueFeature(TypeRef typeRef, const ValueHolder& valueHolder)
    : ValueFeature(std::move(typeRef))
    , m_value(valueHolder) {}

const babelwires::ValueHolder& babelwires::ChildValueFeature::doGetValue() const {
    return m_value;
}

const babelwires::SimpleValueFeature& babelwires::ChildValueFeature::getRootValueFeature() const {
    assert(getOwner() && "You can only get the RootValueFeature from a ValueFeature in a hierarchy.");
    const ValueFeature *const owner = getOwner()->as<ValueFeature>();
    assert(owner && "The owner of a ChildValueFeature must be a ValueFeature");
    return owner->getRootValueFeature();
}

babelwires::SimpleValueFeature& babelwires::ChildValueFeature::getRootValueFeature() {
    assert(getOwner() && "You can only get the RootValueFeature from a ValueFeature in a hierarchy.");
    ValueFeature *const owner = getOwnerNonConst()->as<ValueFeature>();
    assert(owner && "The owner of a ChildValueFeature must be a ValueFeature");
    return owner->getRootValueFeature();
}
