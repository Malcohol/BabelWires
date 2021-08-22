/**
 * A singleton which ensures that fieldIdentifiers are unique in the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/Path/fieldIdentifier.hpp"

#include "Common/Serialization/serializable.hpp"
#include "Common/uuid.hpp"

#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace babelwires {

    /// Associates field names with field identifiers.
    /// This has support for acting as a "thread-safe singleton".
    /// Singletons are usually a mistake and for the most part, "dependency injection" has been used to provide
    /// the project with its dependencies. However, passing the registry through to every use of FeaturePath was
    /// just too painful, so a singleton was adopted in this case.
    class FieldNameRegistry : public Serializable {
      public:
        SERIALIZABLE(FieldNameRegistry, "fieldMetadata", void, 1);
        FieldNameRegistry();
        FieldNameRegistry(FieldNameRegistry&&);
        FieldNameRegistry& operator=(FieldNameRegistry&&);

        virtual ~FieldNameRegistry();

        enum class Authority {
            /// We're registering a field in code.
            isAuthoritative,

            /// We're registering a field from deserialized data.
            isProvisional,

            /// We're populating a temporary registry.
            isTemporary
        };

        /// Give the FieldIdentifier a unique index so it can be later used to look-up the name.
        /// Returns a FieldIdentifier with a modified discriminator.
        FieldIdentifier addFieldName(FieldIdentifier identifier, const std::string& name, const Uuid& uuid,
                                     Authority authority);

        /// Get the name from the given identifier.
        std::string getFieldName(FieldIdentifier identifier) const;

      public:
        // For use during serialization/deserialization:

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// Information stored about a field.
        using ValueType = std::tuple<FieldIdentifier, const std::string*, const Uuid*>;

        /// Extract the information about a field from a local FieldNameRegistry.
        /// This can throw a ParseException if the contents are invalid.
        ValueType getDeserializedFieldData(FieldIdentifier identifier) const;

      public:
        // Singleton stuff:

        /// Access the contents of the singleton instance within the scope of this object.
        /// This holds a lock, so ensure its lifetime is minimal.
        class ReadAccess {
          public:
            ReadAccess(std::shared_mutex* mutex, const FieldNameRegistry* registry);
            ReadAccess(ReadAccess&& other);
            ~ReadAccess();
            const FieldNameRegistry* operator->() const { return m_registry; }

          private:
            std::shared_mutex* m_mutex;
            const FieldNameRegistry* m_registry;
        };

        /// Access the contents of the singleton instance within the scope of this object.
        /// This holds a lock, so ensure its lifetime is minimal.
        class WriteAccess {
          public:
            WriteAccess(std::shared_mutex* mutex, FieldNameRegistry* registry);
            WriteAccess(WriteAccess&& other);
            ~WriteAccess();
            FieldNameRegistry* operator->() const { return m_registry; }

          private:
            std::shared_mutex* m_mutex;
            FieldNameRegistry* m_registry;
        };

        /// Swap the singleton instance with the provided object.
        static FieldNameRegistry* swapInstance(FieldNameRegistry* reg);

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
            InstanceData(std::string fieldName, Uuid uuid, FieldIdentifier identifier, Authority authority);

            void serializeContents(Serializer& serializer) const override;
            void deserializeContents(Deserializer& deserializer) override;

            std::string m_fieldName;
            Uuid m_uuid;
            FieldIdentifier m_identifier;
            Authority m_authority;
        };

        ///
        const InstanceData* getInstanceData(FieldIdentifier identifier) const;

      private:
        friend const_iterator;

        /// Look up the instance data associated with a field.
        std::unordered_map<Uuid, std::unique_ptr<InstanceData>> m_uuidToInstanceDataMap;

        struct Data {
            std::vector<InstanceData*> m_fieldNames;
        };

        /// For each field, we can index associated data.
        std::unordered_map<FieldIdentifier, Data> m_fieldData;

      private:
        /// Lifetime of this is managed externally.
        static FieldNameRegistry* s_singletonInstance;

        /// A mutex that controls access to the singleton's contents.
        static std::shared_mutex s_mutex;
    };

    /// Creates a FieldNameRegistry and sets it to be the singleton instance.
    class FieldNameRegistryScope {
      public:
        FieldNameRegistryScope();
        ~FieldNameRegistryScope();

      private:
        FieldNameRegistry m_currentInstance;
        FieldNameRegistry* m_previousInstance;
    };

} // namespace babelwires

class babelwires::FieldNameRegistry::const_iterator {
  public:
    using InnerIterator = decltype(babelwires::FieldNameRegistry::m_uuidToInstanceDataMap)::const_iterator;
    using ValueType = FieldNameRegistry::ValueType;

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
