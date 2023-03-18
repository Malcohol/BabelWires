/**
 * A ValueFeature is a feature which stores a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

std::string babelwires::ValueFeature::getValueType() const {
    // Future proofing, while I consider this approach.
    assert((doGetValueType().size() <= 4) && "The value type must be 4 characters or shorter");
    return doGetValueType();
}

bool babelwires::ValueFeature::isCompatible(const ValueFeature& other) {
    return getValueType() == other.getValueType();
}

void babelwires::ValueFeature::assign(const ValueFeature& other) {
    if (!isCompatible(other)) {
        throw ModelException() << "Assigning an incompatible value";
    }
    doAssign(other);
}

void babelwires::ValueFeature::doSetToDefaultNonRecursive() {
    setToDefault();
}
