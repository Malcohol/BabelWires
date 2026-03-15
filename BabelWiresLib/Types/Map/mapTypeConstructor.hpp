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
    /// Construct a new MapType from a source and target type and a default fallback kind.
    class MapTypeConstructor : public TypeConstructor {
      public:
        DOWNCASTABLE(MapTypeConstructor, TypeConstructor);
        /// Note that the we don't represent the default in the name.
        TYPE_CONSTRUCTOR("Map", "{0}\u21C9{1}", "7b45de96-b355-4d87-a1b1-a6305b660716", 1);

        ResultT<TypePtr> constructType(const TypeSystem& typeSystem, TypeExp newTypeExp, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        /// Convenience method.
        static TypeExp makeTypeExp(TypeExp sourceTypeExp, TypeExp targetTypeExp, MapEntryData::Kind fallbackKind = MapEntryData::Kind::All21);

      private:
        /// Returns an error if the arguments are not of the expected type.
        static ResultT<MapEntryData::Kind> extractValueArguments(const TypeSystem& typeSystem, const std::vector<ValueHolder>& valueArguments);
    };
} // namespace babelwires
