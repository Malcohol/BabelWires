/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>

namespace babelwires {


    /// A ValueFeature is a feature which provides access to a value.
    class ValueFeature : public Feature {
      public:
        /// Construct a ValueFeature which carries values of the given type.
        ValueFeature(TypeRef typeRef);
    };
}
