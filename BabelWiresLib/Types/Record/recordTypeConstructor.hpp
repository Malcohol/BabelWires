/**
 * Construct a simple record from types and fieldIds.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Record/fieldIdValue.hpp>

namespace babelwires {

    /// Construct a simple record from types and fieldIds.
    class RecordTypeConstructor : public TypeConstructor {
      public:
        // Example: Record{a, b, c : String, Integer, String}
        TYPE_CONSTRUCTOR("Record", "Record{{[0|, ] : {0|, }}}", "295459cc-9485-4526-86ac-e8f27e4e7667", 1);

        TypeConstructorResult constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                            const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const override;

        /// Convenience method.
        template <typename... ARGS> static TypeRef makeTypeRef(ARGS&&... args) {
            std::vector<ValueHolder> valueArguments;
            std::vector<TypeRef> typeArguments;
            valueArguments.reserve(sizeof...(args) / 2);
            typeArguments.reserve(sizeof...(args) / 2);
            Detail::addToArrays(valueArguments, typeArguments, std::forward<ARGS>(args)...);
            return TypeRef(getThisIdentifier(), {std::move(typeArguments), std::move(valueArguments)});
        }

        struct Detail {
            /// Construct a TypeRef for a record type with the given field names and types.
            /// The field names are given as ShortIds, and the types as TypeRefs.
            static void addToArrays(std::vector<ValueHolder>& fieldNames, std::vector<TypeRef>& fieldTypes) {}

            template <typename... ARGS>
            static void addToArrays(std::vector<ValueHolder>& fieldNames, std::vector<TypeRef>& fieldTypes,
                                    ShortId fieldA, TypeRef typeRefA, ARGS&&... args) {
                fieldNames.emplace_back(FieldIdValue(fieldA));
                fieldTypes.emplace_back(typeRefA);
                addToArrays(fieldNames, fieldTypes, std::forward<ARGS>(args)...);
            };
        };
    };
} // namespace babelwires
