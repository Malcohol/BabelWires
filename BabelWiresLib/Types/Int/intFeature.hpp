/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    class IntFeature2 : public SimpleValueFeature {
      public:
        IntFeature2();
        IntFeature2(IntValue::NativeType defaultValue);
        IntFeature2(IntValue::NativeType minValue, IntValue::NativeType maxValue, IntValue::NativeType defaultValue = 0);

        IntValue::NativeType get() const;
        void set(IntValue::NativeType value);
    };
} // namespace babelwires
