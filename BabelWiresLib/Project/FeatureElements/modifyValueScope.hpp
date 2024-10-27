/**
 * A ModifyValueScope encloses the modification of values within the input feature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>

namespace babelwires {
    class FeatureElement;
    class ValueTreeRoot;

    class ModifyValueScope {
      public:
        ModifyValueScope() = default;
        ModifyValueScope(Path pathToRootValue, ValueTreeRoot* rootValueFeature);

      public:
        Path m_pathToRootValue;
        ValueTreeRoot* m_valueTreeRoot = nullptr;
    };
} // namespace babelwires