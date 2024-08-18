/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {

    /// Common between IntTypes constructed with a range and/or default.
    class IntType : public Type {
      public:
        IntType(Range<IntValue::NativeType> range = Range<IntValue::NativeType>(), IntValue::NativeType defaultValue = 0);

        /// Get the range of valid values.
        Range<IntValue::NativeType> getRange() const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        IntValue::NativeType m_defaultValue;
        Range<IntValue::NativeType> m_range;
    };

    /// The standard Int type which has a default of zero and allows the full range of IntValue::NativeType.
    class DefaultIntType : public IntType {
      public:
        DefaultIntType();

        PRIMITIVE_TYPE("int", "Integer", "90ed4c0c-2fa1-4373-9b67-e711358af824", 1);
    };

    /// An Int type which covers the range between 0 and the maximum NativeInt value.
    class NonNegativeIntType : public IntType {
      public:
        NonNegativeIntType();

        PRIMITIVE_TYPE("NonNegInt", "Non-Negative Integer", "33d35b26-d8ab-4af9-bc63-958ffb37b261", 1);
    };
}
