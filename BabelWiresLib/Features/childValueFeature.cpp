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

