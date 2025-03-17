/**
 * A TypeConstructor which constructs a new SumType from a pair of types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a new SumType from a pair of types.
    class SumTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("Sum", "{0}+{1}", "e9978340-49d9-49f3-922a-3c367f5feaec", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        // Convenience method
        static TypeRef makeTypeRef(std::vector<TypeRef> types);
    };
} // namespace babelwires
