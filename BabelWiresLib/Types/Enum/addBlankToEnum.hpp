/**
 * A type constructor which adds a blank value to an existing enum
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>

namespace babelwires {
    // TODO This is redundant. Remove it and use EnumUnionTypeConstructor and EnumAtomTypeConstructor instead.
    /// A unary type constructor which adds a blank value to an existing enum.
    /// This is useful in map targets to indicate when the map should exclude an entry, rather than select a mapped
    /// value.
    /// Applying AddBlankToEnum to an enum which already has a blank returns an enum with the same values.
    class AddBlankToEnum : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("AddBlankToEnum", "{0}_", "bd5af7a5-4a75-4807-a3d8-93851e1a7d00", 1);

        /// The blank value that gets added to enums.
        /// Note that the blank value is only permitted at the end of the enum.
        static ShortId getBlankValue();

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        /// Convenience method.
        static TypeRef makeTypeRef(TypeRef enumTypeRef);

        /// Add a blank to the values unless one is already there.
        static EnumType::ValueSet ensureBlankValue(const EnumType::ValueSet& srcValues);
    };
} // namespace babelwires
