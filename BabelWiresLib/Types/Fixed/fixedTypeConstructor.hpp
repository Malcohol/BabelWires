/**
 * A TypeConstructor which constructs a fixed-point type.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    class BABELWIRESLIB_API FixedTypeConstructor : public TypeConstructor {
      public:
        DOWNCASTABLE(FixedTypeConstructor, TypeConstructor);

        TYPE_CONSTRUCTOR("Fixed", "Fixed{[0] [1]..[2] [3]}", "2cd6a5a8-687d-4dc7-8dba-f0afde070b21", 1);

        ResultT<TypePtr> constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                       const TypeConstructorArguments& arguments,
                                       const std::vector<TypePtr>& resolvedTypeArguments) const override;

        static TypeExp makeTypeExp(int precision, Fixed min, Fixed max, Fixed defaultValue);

      private:
        static ResultT<std::tuple<int, Range<Fixed::NativeType>, Fixed::NativeType>>
        extractValueArguments(const std::vector<ValueHolder>& valueArguments);
    };
} // namespace babelwires
