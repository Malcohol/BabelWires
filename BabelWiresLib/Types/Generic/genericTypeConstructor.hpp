/**
 * A Type constructor which constructs a GenericType from a TypeRef and a number of type variables.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a GenericType from a TypeRef and a number of type variables.
    class GenericTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("Gen", "Gen<[0]>", "56ff5848-9be4-4288-a893-eec7ef15e675", 1);

        TypeConstructorResult constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const override;

        /// Convenience method.
        static TypeRef makeTypeRef(unsigned int numVariables = 1);

        /// Throws a TypeSystem exception if the arguments are not of the expected type.
        static unsigned int
        extractValueArguments(const std::vector<EditableValueHolder>& valueArguments);
    };
} // namespace babelwires
