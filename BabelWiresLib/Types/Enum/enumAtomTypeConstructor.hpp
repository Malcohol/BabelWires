/**
 * A type constructor which creates an enum with a single value.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

namespace babelwires {
    /// Creates an EnumType with a single value.
    class EnumAtomTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("EnumAtom", "[0]", "cb88eeac-92e6-4a77-aa73-04db08c0d628", 1);

        TypePtr constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        /// Convenience method.
        static TypeRef makeTypeRef(EnumValue value);
    };
} // namespace babelwires
