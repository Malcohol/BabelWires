/**
 * A singleton that associates global metadata with identifiers.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Identifiers/identifier.hpp"

#include "Common/Serialization/serializable.hpp"
#include "Common/uuid.hpp"

#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace babelwires {

    /// A singleton that associates global metadata with identifiers.
    /// This has support for acting as a "thread-safe singleton".
    /// Singletons are usually a mistake and for the most part, "dependency injection" has been used to provide
    /// the project with its dependencies. However, passing the registry through to every use of FeaturePath was
    /// just too painful, so a singleton was adopted in this case.
    class IdentifierRegistry : public Serializable {
      public:
        SERIALIZABLE(IdentifierRegistry, "fieldMetadata", void, 1);
        IdentifierRegistry();
        IdentifierRegistry(IdentifierRegistry&&);
        IdentifierRegistry& operator=(IdentifierRegistry&&);

        virtual ~IdentifierRegistry();

        enum class Authority {
            /// We're registering an identifier in code.
            isAuthoritative,

            /// We're registering an identifier from deserialized data.
            isProvisional,

            /// We're populating a temporary registry.
            isTemporary
        };

        /// Give the Identifier a unique index so it can be later used to look-up the name.
        /// Returns a Identifier with a modified discriminator.
        Identifier addIdentifierWithMetadata(Identifier identifier, const std::string& name, const Uuid& uuid,
                                     Authority authority);

        /// Get the name from the given identifier.
        std::string getName(Identifier identifier) const;

      public:
        // For use during serialization/deserialization:

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// Information stored about an identifier.
        using ValueType = std::tuple<Identifier, const std::string*, const Uuid*>;

        /// Extract the information about an identifier from a local IdentifierRegistry.
        /// This can throw a ParseException if the contents are invalid.
        ValueType getDeserializedIdentifierData(Identifier identifier) const;

      public:
        // Singleton stuff:

        /// Access the contents of the singleton instance within the scope of this object.
        /// This holds a lock, so ensure its lifetime is minimal.
        class ReadAccess {
          public:
            ReadAccess(std::shared_mutex* mutex, const IdentifierRegistry* registry);
            ReadAccess(ReadAccess&& other);
            ~ReadAccess();
            const IdentifierRegistry* operator->() const { return m_registry; }

          private:
            std::shared_mutex* m_mutex;
            const IdentifierRegistry* m_registry;
        };

        /// Access the contents of the singleton instance within the scope of this object.
        /// This holds a lock, so ensure its lifetime is minimal.
        class WriteAccess {
          public:
            WriteAccess(std::shared_mutex* mutex, IdentifierRegistry* registry);
            WriteAccess(WriteAccess&& other);
            ~WriteAccess();
            IdentifierRegistry* operator->() const { return m_registry; }

          private:
            std::shared_mutex* m_mutex;
            IdentifierRegistry* m_registry;
        };

        /// Swap the singleton instance with the provided object.
        static IdentifierRegistry* swapInstance(IdentifierRegistry* reg);

        /// Gain read access to the registry.
        static ReadAccess read();

        /// Gain write access to the registry.
        static WriteAccess write();

      public:
        /// Iteration.
        class const_iterator;
        const_iterator begin() const;
        const_iterator end() const;

      protected:
        struct InstanceData : Serializable {
            SERIALIZABLE(InstanceData, "field", void, 1);
            InstanceData();
            InstanceData(std::string fieldName, Uuid uuid, Identifier identifier, Authority authority);

            void serializeContents(Serializer& serializer) const override;
            void deserializeContents(Deserializer& deserializer) override;

            std::string m_fieldName;
            Uuid m_uuid;
            Identifier m_identifier;
            Authority m_authority;
        };

        ///
        const InstanceData* getInstanceData(Identifier identifier) const;

      private:
        friend const_iterator;

        /// Look up the instance data associated with a uuid.
        std::unordered_map<Uuid, std::unique_ptr<InstanceData>> m_uuidToInstanceDataMap;

        struct Data {
            std::vector<InstanceData*> m_instanceDatas;
        };

        /// For each identifier, we can index associated data.
        std::unordered_map<Identifier, Data> m_instanceDatasFromIdentifier;

      private:
        /// Lifetime of this is managed externally.
        static IdentifierRegistry* s_singletonInstance;

        /// A mutex that controls access to the singleton's contents.
        static std::shared_mutex s_mutex;
    };

    /// Creates a IdentifierRegistry and sets it to be the singleton instance.
    class IdentifierRegistryScope {
      public:
        IdentifierRegistryScope();
        ~IdentifierRegistryScope();

      private:
        IdentifierRegistry m_currentInstance;
        IdentifierRegistry* m_previousInstance;
    };

} // namespace babelwires

class babelwires::IdentifierRegistry::const_iterator {
  public:
    using InnerIterator = decltype(babelwires::IdentifierRegistry::m_uuidToInstanceDataMap)::const_iterator;
    using ValueType = IdentifierRegistry::ValueType;

    const_iterator(InnerIterator it)
        : m_it(it) {}

    ValueType operator*() const;

    bool operator==(const const_iterator& other) const { return m_it == other.m_it; }
    bool operator!=(const const_iterator& other) const { return m_it != other.m_it; }

    const_iterator& operator++() {
        ++m_it;
        return *this;
    }

  private:
    InnerIterator m_it;
};
