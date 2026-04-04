
/**
 * A TypeConstructor which constructs a new TupleType from a vector of types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a new TupleType from a vector of types.
    class BABELWIRESLIB_API TupleTypeConstructor : public TypeConstructor {
      public:
        DOWNCASTABLE(TupleTypeConstructor, TypeConstructor);
        // TupleTypes are variadic.
        TYPE_CONSTRUCTOR("Tuple", "{0|*}", "e55f3a4d-9b62-4f54-964f-bca4a42e8f68", 1);

        ResultT<TypePtr> constructType(const TypeSystem& typeSystem, TypeExp newTypeExp, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        // Convenience method
        static TypeExp makeTypeExp(std::vector<TypeExp> types);
    };
} // namespace babelwires
