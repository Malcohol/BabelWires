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
    class IntFeature : public SimpleValueFeature {
        IntFeature();
        IntFeature(IntValue::NativeType defaultValue);
        IntFeature(Range<IntValue::NativeType> range = Range<IntValue::NativeType>(),
                   IntValue::NativeType defaultValue = 0);

        IntValue::NativeType get() const;
        void set(IntValue::NativeType value);
    };
} // namespace babelwires
