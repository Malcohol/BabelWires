/**
 * Construct a simple record from types and fieldIds.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {

    /// Construct a simple record from types and fieldIds.
    class RecordTypeConstructor : public TypeConstructor {
      public:
        // Example: Record{a, b, c : Int, Int, Int}
        TYPE_CONSTRUCTOR("Record", "Record{{{0|, } : [0|, ]}}", "295459cc-9485-4526-86ac-e8f27e4e7667", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                            const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;
    };
} // namespace babelwires
