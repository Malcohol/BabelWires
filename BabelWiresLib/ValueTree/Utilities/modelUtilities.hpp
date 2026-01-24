/**
 * Convenience functions for working with features.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <BaseLib/common.hpp>

namespace babelwires {
    /// Convenience functions supporting ranged for loop.
    /// E.g. for(auto&& f : getChildRange(compoundFeature)) {...}
    template <typename COMPOUND_FEATURE> struct ValueTreeChildRange;
    template <typename COMPOUND_FEATURE> ValueTreeChildRange<COMPOUND_FEATURE> getChildRange(COMPOUND_FEATURE* container);
    template <typename COMPOUND_FEATURE> ValueTreeChildRange<COMPOUND_FEATURE> getChildRange(COMPOUND_FEATURE& container);
} // namespace babelwires

#include <BabelWiresLib/ValueTree/Utilities/modelUtilities_inl.hpp>
