/**
 * Convenience functions
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/types.hpp>
#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class Feature;
    class CompoundFeature;

    class ValueFeatureHelper {
      public:
        /// Abstracts ArrayFeature and ValueFeatures holding ArrayTypes.
        static std::tuple<const CompoundFeature*, unsigned int, Range<unsigned int>, unsigned int> getInfoFromArrayFeature(const Feature* f);

        /// Confirm that the feature is a record with the given optional, and return whether it is active or not.
        static std::tuple<const CompoundFeature*, bool> getInfoFromRecordWithOptionalsFeature(const Feature* f, ShortId optionalId);
    };
}
