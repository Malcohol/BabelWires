/**
 * The type of RationalValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

#include <Common/Math/rational.hpp>

namespace babelwires {

    /// Common between RationalTypes constructed with a range and/or default.
    class RationalType : public Type {
      public:
        RationalType(Range<Rational> range = Range<Rational>(), Rational defaultValue = 0);

        /// Get the range of valid values.
        Range<Rational> getRange() const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const Value& v) const override;

        std::string getKind() const override;

      private:
        Rational m_defaultValue;
        Range<Rational> m_range;
    };

    /// The standard Int type which has a default of zero and allows the full range of IntValue::NativeType.
    class DefaultRationalType : public RationalType {
      public:
        DefaultRationalType();

        PRIMITIVE_TYPE("rational", "Rational", "995624d6-6f1d-4407-babd-66ec74989c07", 1);
    };
}
