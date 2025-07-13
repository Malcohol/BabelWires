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
    /// Construct a TypeVariable from two IntValues: The index in the generic type and the number of
    /// generic types to traverse upwards to find the generic type of which this is a variable.
    class TypeVariableTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("TVar", "TVar([0|,])", "33f14749-b652-4d53-808b-8d8c281c310f", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        /// Convenience method.
        static TypeRef makeTypeRef(unsigned int typeVariableIndex = 0, unsigned int numGenericTypeLevels = 0);

      private:
        /// Throws a TypeSystem exception if the arguments are not of the expected type.
        static std::tuple<unsigned int, unsigned int>
        extractValueArguments(const std::vector<EditableValueHolder>& valueArguments);
    };
} // namespace babelwires
