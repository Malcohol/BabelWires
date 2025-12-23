/**
 * Construct a FailureType with a context-specific error message.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a new FailureType from a StringValue containing an error message.
    class FailureTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("Failure", "Failure:\"[0]\"", "5e8bbcc2-e1aa-430f-a87d-b6516730e1ab", 1);

        /// Convenience function which builds a TypeRef describing a FailureType with the given message.
        static TypeRef makeTypeRef(std::string message);

        TypeConstructorResult constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                            const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const override;
    };
} // namespace babelwires
