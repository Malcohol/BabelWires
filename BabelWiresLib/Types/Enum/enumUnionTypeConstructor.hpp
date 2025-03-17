/**
 * A type constructor which constructs an EnumType by combining other EnumTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

namespace babelwires {
    /// Constructs an EnumType by combining other EnumTypes.
    class EnumUnionTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("EnumUnion", "({0| U })", "92eade8a-5315-419f-b3c6-71424ff6ea49", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        /// Convenience method for binary unions
        static TypeRef makeTypeRef(TypeRef enumA, TypeRef enumB);
    };
} // namespace babelwires
