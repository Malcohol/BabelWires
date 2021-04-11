#pragma once

#include "Common/Serialization/deserializationRegistry.hpp"

namespace babelwires {

    /// This class has a static registry into which all serializable classes are automatically registered.
    /// NOTE: This will not work for dynamically linked executable.
    /// Any instance of this class can be used to look up the classes.
    class AutomaticDeserializationRegistry : public DeserializationRegistry {
      public:
        struct AutomaticEntry : Entry {
            AutomaticEntry(Factory factory, std::string_view serializationType, VersionNumber version,
                           const void* baseClassTag);

            /// These entries form a linked list.
            AutomaticEntry* m_next = nullptr;
        };

        const Entry* findEntry(std::string_view typeName) const override;
    };

} // namespace babelwires
