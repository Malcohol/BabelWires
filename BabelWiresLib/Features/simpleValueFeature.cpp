/**
 * A SimpleValueFeature is a ValueFeature which owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SimpleValueFeature::SimpleValueFeature(TypeRef typeRef)
    : ValueFeature(std::move(typeRef)) {}

const babelwires::ValueHolder& babelwires::SimpleValueFeature::doGetValue() const {
    // Not sure if this assert is necessary.
    assert(m_value && "The SimpleValueFeature has not been initialized");
    return m_value;
}

const babelwires::SimpleValueFeature& babelwires::SimpleValueFeature::getRootValueFeature() const {
    return *this;
}

babelwires::SimpleValueFeature& babelwires::SimpleValueFeature::getRootValueFeature() {
    return *this;
}
