/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Identifiers/identifier.hpp"
#include "Common/Registry/registry.hpp"

#include <vector>

namespace babelwires {

    /// Defines the set of values available for enum features.
    /// See convenience class EnumWithCppEnum for a Enum which carries a corresponding C++ enum.
    /// TODO: Allow the user to create maps between Integers and Enums or Enums and Enums.
    class Enum : public RegistryEntry {
      public:
        /// The set of values. We use Identifiers to get versionable serialization support.
        /// Note: This matches the result of REGISTERED_ID_VECTOR.
        using EnumValues = std::vector<Identifier>;

        /// Enums can be registered in a registry, so they need their own identifier and version.
        /// They also need a set of values and a way of identifying the default.
        /// The values object can be the "output" of the REGISTERED_ID_VECTOR macro.
        Enum(LongIdentifier identifier, VersionNumber version, const EnumValues& values,
             unsigned int indexOfDefaultValue);

        /// Get the set of available enum values.
        const EnumValues& getEnumValues() const;

        /// Get the index of the default value in the vector of enum values.
        unsigned int getIndexOfDefaultValue() const;

        /// Get the index within EnumValues of the given id.
        unsigned int getIndexFromIdentifier(Identifier id) const;

        /// Get the identifier within EnumValues at the given index.
        Identifier getIdentifierFromIndex(unsigned int index);

      private:
        const EnumValues& m_values;
        unsigned int m_indexOfDefaultValue;
    };

    /// Enums should be available via the EnumRegistry, but also need to be available when constructing
    /// EnumFeatures. Using this as a base class ensures that an Enum instance can be statically queried
    /// when it has been registered in the EnumRegistry.
    template <typename E> class RegisteredEnum : public Enum {
      public:
        RegisteredEnum(LongIdentifier identifier, VersionNumber version, const EnumValues& values,
                       unsigned int indexOfDefaultValue)
            : Enum(identifier, version, values, indexOfDefaultValue) {}

        /// Get the instance of this class which is registered in the EnumRegistered.
        static const E* getRegisteredInstance() { return s_registeredInstance; }

        void onRegistered() override {
            assert((s_registeredInstance == nullptr) && "Only one entry of each enum type can be registered");
            s_registeredInstance = static_cast<E*>(this);
        }

      private:
        static const E* s_registeredInstance;
    };

    template <typename E> const E* RegisteredEnum<E>::s_registeredInstance = nullptr;

    /// The EnumRegistry allows registered Enums to be found.
    /// The intention is to support user-defined maps between Enums and integers.
    /// Right now they only have an identifier, but perhaps it would be useful if they
    /// had a path.
    class EnumRegistry : public Registry<Enum> {
      public:
        EnumRegistry();
    };
} // namespace babelwires