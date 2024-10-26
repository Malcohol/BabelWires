/**
 * A ModifyFeatureScope encloses the modification of values within the input feature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/modifyFeatureScope.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

babelwires::ModifyFeatureScope::ModifyFeatureScope(Path pathToRootValue, ValueTreeRoot* rootValueFeature)
    : m_pathToRootValue(std::move(pathToRootValue))
    , m_rootValueFeature(rootValueFeature) {}

