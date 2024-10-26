/**
 * Convenience functions for working with features.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Features/valueTreeNode.hpp>

#include <Common/types.hpp>

namespace babelwires {
    /// Convenience functions supporting ranged for loop.
    /// E.g. for(auto&& f : subfeatures(compoundFeature)) {...}
    template <typename COMPOUND_FEATURE> struct SubFeaturesRange;
    template <typename COMPOUND_FEATURE> SubFeaturesRange<COMPOUND_FEATURE> subfeatures(COMPOUND_FEATURE* container);
    template <typename COMPOUND_FEATURE> SubFeaturesRange<COMPOUND_FEATURE> subfeatures(COMPOUND_FEATURE& container);
} // namespace babelwires

#include <BabelWiresLib/Features/Utilities/modelUtilities_inl.hpp>
