/**
 * A TypeConstructor which constructs a text type with a given maximum length.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/babelWiresLibExport.hpp>

namespace babelwires {
    /// Construct a new TextType from three IntValues: min, max and default.
    class BABELWIRESLIB_API TextTypeConstructor : public TypeConstructor {
      public:
        DOWNCASTABLE(TextTypeConstructor, TypeConstructor);

        TYPE_CONSTRUCTOR("Text", "Text[[[0]]]", "1b4168ff-1621-4cb3-8be3-828dab44e86d", 1);

        ResultT<TypePtr> constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                       const TypeConstructorArguments& arguments,
                                       const std::vector<TypePtr>& resolvedTypeArguments) const override;

        /// Convenience method.
        static TypeExp makeTypeExp(IntValue::NativeType maxLength);

      private:
        /// Returns an error if the arguments are not of the expected type.
        static ResultT<IntValue::NativeType> extractValueArguments(const std::vector<ValueHolder>& valueArguments);
    };
} // namespace babelwires