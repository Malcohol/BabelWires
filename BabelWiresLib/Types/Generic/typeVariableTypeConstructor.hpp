/**
 * A TypeConstructor which constructs a TypeVariable from two IntValues and an optional typeRef.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>

#include <optional>

namespace babelwires {
    /// Construct a TypeVariable from two IntValues and an optional typeRef.
    /// If there is a typeRef argument, it means the variable has been instantiated and the
    /// type constructor expression is a pure wrapper for the instantiated type.
    /// The two IntValues represent the index of the variable in the generic type and the number of
    /// generic types to traverse upwards to find the generic type of which this is a variable.
    class TypeVariableTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("TVar", "TVar([0|,])", "33f14749-b652-4d53-808b-8d8c281c310f", 1);

        TypeConstructorResult constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const override;

        /// Convenience method.
        static TypeRef makeTypeRef(unsigned int typeVariableIndex = 0, unsigned int numGenericTypeLevels = 0);

        /// Throws a TypeSystem exception if the arguments are not of the expected type.
        static TypeVariableData extractValueArguments(const std::vector<EditableValueHolder>& valueArguments);
    };
} // namespace babelwires
