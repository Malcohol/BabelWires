/**
 * MapType is the type for MapData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>

namespace babelwires {

    /// 
    class MapType : public Type {
      public:
        MapType(TypeRef sourceTypeRef, TypeRef targetTypeRef, MapEntryData::Kind fallbackKind = MapEntryData::Kind::All21);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        const TypeRef& getSourceTypeRef() const;
        const TypeRef& getTargetTypeRef() const;

      private:
        TypeRef m_sourceTypeRef;
        TypeRef m_targetTypeRef;
        MapEntryData::Kind m_fallbackKind;
    };
} // namespace babelwires
