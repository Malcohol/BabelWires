/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

std::tuple<const babelwires::CompoundFeature*, unsigned int, babelwires::Range<unsigned int>, unsigned int> babelwires::ValueFeatureHelper::getInfoFromArrayFeature(const Feature* f) {
    if (f) {
        if (auto arrayFeature = f->as<const ArrayFeature>()) {
            return { arrayFeature, arrayFeature->getNumFeatures(), arrayFeature->getSizeRange(), arrayFeature->getSizeRange().m_min };
        } else if (auto valueFeature = f->as<const ValueFeature>()) {
            if (auto arrayType = valueFeature->getType().as<ArrayType>()) {
                return { valueFeature, arrayType->getNumChildren(valueFeature->getValue()), arrayType->getSizeRange(), arrayType->getInitialSize()};
            } 
        }
    }
    return {};
}
