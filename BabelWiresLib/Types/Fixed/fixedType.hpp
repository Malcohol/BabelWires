/**
 * The type of fixed-point values.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

namespace babelwires {

    class BABELWIRESLIB_API FixedType : public Type {
      public:
        DOWNCASTABLE(FixedType, Type);

        static constexpr int s_defaultPrecision = 2;

        FixedType(TypeExp&& typeExpOfThis, int precision = s_defaultPrecision,
                  Range<Fixed::NativeType> range = Range<Fixed::NativeType>(), Fixed::NativeType defaultValue = 0);

        int getPrecision() const;
        Range<Fixed::NativeType> getRange() const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        std::string getFlavour() const override;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        Fixed::NativeType m_defaultValue;
        Range<Fixed::NativeType> m_range;
        int m_precision;
    };

    class BABELWIRESLIB_API DefaultFixedType : public FixedType {
      public:
        DOWNCASTABLE(DefaultFixedType, FixedType);

        DefaultFixedType();

        REGISTERED_TYPE("fixed", "Fixed", "7f0e7bb1-68d0-48ad-a5c9-81db7a43f89a", 1);
    };

} // namespace babelwires
