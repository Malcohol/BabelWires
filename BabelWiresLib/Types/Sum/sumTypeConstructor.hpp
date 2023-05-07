/**
 * A TypeConstructor which constructs a map type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    /// Construct a new SumType from a source and target type and a default fallback kind.
    class SumTypeConstructor : public TypeConstructor {
      public:
        /// Note that the we don't represent the default in the name.
        TYPE_CONSTRUCTOR("Sum", "{0}+{1}", "e9978340-49d9-49f3-922a-3c367f5feaec", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& argumentsA,
                                          const TypeConstructorArguments& argumentsB) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments,
                                          const TypeRef& other) const override;

      private:
        SubtypeOrder compareSubtypeHelperInternal(const TypeSystem& typeSystem, const std::vector<TypeRef>& summandsA,
                                          const std::vector<TypeRef>& summandsB) const;
    };
} // namespace babelwires
