/**
 * A feature which manages a single IntValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    class IntFeature : public SimpleValueFeature {
      public:
        IntFeature();
        IntFeature(IntValue::NativeType defaultValue);
        IntFeature(IntValue::NativeType minValue, IntValue::NativeType maxValue, IntValue::NativeType defaultValue = 0);

        IntValue::NativeType get() const;
        void set(IntValue::NativeType value);
    };
} // namespace babelwires
