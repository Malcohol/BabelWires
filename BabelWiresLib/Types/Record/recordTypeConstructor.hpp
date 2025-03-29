/**
 * 
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

// TODO Temporarily using enum values to hold field IDs.
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

namespace babelwires {

    // TODO Temporarily using enum values to hold field IDs.
    using FieldIdValue = EnumValue;

    /// Construct a simple record from types and fieldIds.
    class RecordTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("Record", "Record{{...}}", "295459cc-9485-4526-86ac-e8f27e4e7667", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                            const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;
    };
} // namespace babelwires
