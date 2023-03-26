/**
 * An EnumFeature exposes an enum in the project, and allows the user to make a choice from a set of named values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <vector>

namespace babelwires {
    class Enum;

    /// A subclass of SimpleValueFeature which provides convenient methods to access
    /// and modify the EnumValue carried by the value.
    class EnumFeature : public SimpleValueFeature {
      public:
        EnumFeature(TypeRef e);

        /// Get the enum which determines the values of this feature.
        const Enum& getEnum() const;

        /// Get the current value of the feature.
        ShortId get() const;

        /// Set the value in the feature.
        void set(ShortId value);
    };

} // namespace babelwires
