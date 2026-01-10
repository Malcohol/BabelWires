/**
 * The type of RationalValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

#include <Common/Math/rational.hpp>

namespace babelwires {

    /// Common between RationalTypes constructed with a range and/or default.
    class RationalType : public Type {
      public:
        RationalType(TypeExp typeExp, Range<Rational> range = Range<Rational>(), Rational defaultValue = 0);

        /// Get the range of valid values.
        Range<Rational> getRange() const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        std::string getFlavour() const override;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        Rational m_defaultValue;
        Range<Rational> m_range;
    };

    /// The standard Int type which has a default of zero and allows the full range of IntValue::NativeType.
    class DefaultRationalType : public RationalType {
      public:
        DefaultRationalType();

        REGISTERED_TYPE("rational", "Rational", "995624d6-6f1d-4407-babd-66ec74989c07", 1);
    };
}
