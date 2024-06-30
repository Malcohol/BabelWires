/**
 * A set of useful functions for interacting with features of ArrayType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/InstanceOf/instanceOfArrayType.hpp>

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

unsigned int babelwires::ArrayFeatureUtils::getArraySize(const babelwires::ValueFeature& arrayFeature) {
    return arrayFeature.getNumFeatures();
}

void babelwires::ArrayFeatureUtils::setArraySize(babelwires::ValueFeature& arrayFeature, unsigned int newSize) {
    const auto& type = arrayFeature.getType().is<babelwires::ArrayType>();
    const auto& typeSystem = babelwires::RootFeature::getTypeSystemAt(arrayFeature);
    babelwires::ValueHolder value = arrayFeature.getValue();
    value.copyContentsAndGetNonConst();
    type.setSize(typeSystem, value, newSize);
    arrayFeature.setValue(value);
}

const babelwires::ValueFeature& babelwires::ArrayFeatureUtils::getChild(const babelwires::ValueFeature& arrayFeature, unsigned int index) {
    return arrayFeature.getFeature(index)->is<babelwires::ValueFeature>();
}

babelwires::ValueFeature& babelwires::ArrayFeatureUtils::getChild(babelwires::ValueFeature& arrayFeature, unsigned int index) {
    return arrayFeature.getFeature(index)->is<babelwires::ValueFeature>();
}
