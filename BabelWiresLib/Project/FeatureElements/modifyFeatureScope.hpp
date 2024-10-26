/**
 * A ModifyFeatureScope encloses the modification of values within the input feature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>

namespace babelwires {
    class FeatureElement;
    class SimpleValueFeature;

    class ModifyFeatureScope {
      public:
        ModifyFeatureScope() = default;
        ModifyFeatureScope(Path pathToRootValue, SimpleValueFeature* rootValueFeature);

      public:
        Path m_pathToRootValue;
        SimpleValueFeature* m_rootValueFeature = nullptr;
    };
} // namespace babelwires