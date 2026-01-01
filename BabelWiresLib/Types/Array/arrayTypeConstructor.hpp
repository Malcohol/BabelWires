/**
 * A TypeConstructor which constructs an array type with a particular entry type, size range and default size.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a new ArrayType from a TypeExp and three IntValues: min size, max size and default size.
    class ArrayTypeConstructor : public TypeConstructor {
      public:
        /// Note that the default size is not represented in the name.
        TYPE_CONSTRUCTOR("Array", "Array<{0}>[[[0]..[1]]]", "3f8cac9a-2c0b-439f-97ed-bde16874b994", 1);

        TypePtr constructType(const TypeSystem& typeSystem, TypeExp newTypeExp, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        /// Convenience method.
        /// If defaultSize is less than minimumSize, the minimumSize is used.
        static TypeExp makeTypeExp(TypeExp entryType, unsigned int minSize, unsigned int maxSize, unsigned int defaultSize = 0);

      private:
        /// Throws a TypeSystem exception if the arguments are not of the expected type.
        static std::tuple<unsigned int, unsigned int, unsigned int>
        extractValueArguments(const std::vector<ValueHolder>& valueArguments);
    };
} // namespace babelwires
