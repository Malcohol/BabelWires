/**
 * An interface for looking up deserialization information about classes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/types.hpp>
#include <functional>
#include <string_view>

namespace babelwires {

    class Deserializer;

    /// An interface for looking up deserialization information about classes.
    class DeserializationRegistry {
      public:
        using Factory = std::function<void*(Deserializer& deserializer)>;

        /// Instances of this object represent the registration of a single concrete class' deserializingFactory.
        struct Entry {
            Factory m_factory;
            std::string_view m_serializationType;
            VersionNumber m_version = 0;
            /// Defines the SerializableBase class the type inherits from.
            const void* m_baseClassTag = nullptr;
        };

        /// Returns nullptr if an entry cannot be found.
        virtual const Entry* findEntry(std::string_view typeName) const = 0;
    };

} // namespace babelwires
