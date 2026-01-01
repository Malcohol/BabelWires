
/**
 * A TypeConstructor which constructs a new TupleType from a vector of types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a new TupleType from a vector of types.
    class TupleTypeConstructor : public TypeConstructor {
      public:
        // TupleTypes are variadic.
        TYPE_CONSTRUCTOR("Tuple", "{0|*}", "e55f3a4d-9b62-4f54-964f-bca4a42e8f68", 1);

        TypePtr constructType(const TypeSystem& typeSystem, TypeExp newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        // Convenience method
        static TypeExp makeTypeExp(std::vector<TypeExp> types);
    };
} // namespace babelwires
