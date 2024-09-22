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

#include <map>
#include <optional>

namespace babelwires {
    class Feature;
    class CompoundFeature;

    /// Abstract the difference between some features and their value type equivalents, so code doesn't always need to 
    /// handle both cases.
    class ValueFeatureHelper {
      public:
        /// Obtain details about an array.
        static std::tuple<const CompoundFeature*, unsigned int, Range<unsigned int>, unsigned int> getInfoFromArrayFeature(const Feature* f);

        /// Confirm that the feature is a record and return the state of the optionals.
        static std::tuple<const CompoundFeature*, std::map<ShortId, bool>> getInfoFromRecordWithOptionalsFeature(const Feature* f);

        /// Confirm that the feature is a record with variants.
        /// Return whether the tag is currently the selected tag and the set of fields to remove.
        /// If no tag is provided, the default tag is assumed.
        static std::tuple<const CompoundFeature*, bool, std::vector<ShortId>> getInfoFromRecordWithVariantsFeature(const Feature* f, std::optional<ShortId> tagId = {});
    };
}
