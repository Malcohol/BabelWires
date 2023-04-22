/**
 * A feature which manages a single RationalValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>

#include <Common/Math/rational.hpp>

namespace babelwires {
    class RationalFeature : public SimpleValueFeature {
      public:
        RationalFeature();
        RationalFeature(Rational defaultValue);
        RationalFeature(Rational minValue, Rational maxValue, Rational defaultValue = 0);

        Rational get() const;
        void set(Rational value);
    };
} // namespace babelwires
