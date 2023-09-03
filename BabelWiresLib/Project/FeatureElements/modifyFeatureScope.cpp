/**
 * A ModifyFeatureScope encloses the modification of values within the input feature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/modifyFeatureScope.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

babelwires::ModifyFeatureScope::ModifyFeatureScope(FeatureElement* element, FeaturePath pathToRootValue, SimpleValueFeature* rootValueFeature)
    : m_element(element)
    , m_pathToRootValue(std::move(pathToRootValue))
    , m_rootValueFeature(rootValueFeature) {}

babelwires::ModifyFeatureScope::~ModifyFeatureScope() {
    if (m_element != nullptr) {
        m_element->finishModification(*this);
    }
}
