/**
 * Specialized instance handling for MapTypes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Instance/instanceTemplates.hpp>
#include <BabelWiresLib/Instance/instanceUtils.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>

namespace babelwires {
    /// It's useful to have instances for map types even when they do not have a dedicated C++ class.
    /// For example, if they are defined using a type constructor.
    /// The DSL provides a DECLARE_INSTANCE_MAP_FIELD macro which directly uses the following instance template.
    /// Note: The second type parameter is the entry type, not map type.
    template <typename VALUE_FEATURE, typename SOURCE_TYPE, typename TARGET_TYPE>
    class MapInstanceImpl : public InstanceCommonBase<VALUE_FEATURE, MapType> {
      public:
        MapInstanceImpl(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, MapType>(valueFeature) {}

        // TODO Add access and update methods.
    };
} // namespace babelwires
