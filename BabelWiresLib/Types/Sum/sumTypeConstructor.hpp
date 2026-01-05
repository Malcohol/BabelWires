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
        // SumTypes are variadic.
        TYPE_CONSTRUCTOR("Sum", "{0|+}", "e9978340-49d9-49f3-922a-3c367f5feaec", 1);

        TypePtr constructType(const TypeSystem& typeSystem, TypeExp newTypeExp, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        // Convenience method
        static TypeExp makeTypeExp(std::vector<TypeExp> types);
    };
} // namespace babelwires
