/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/Path/fieldIdentifier.hpp"
#include "Common/Registry/registry.hpp"

#include <vector>

// Implementation detail.
#define ENUM_SELECT_FIRST_ARGUMENT(A, B, C) A,
#define ENUM_ARGUMENTS_AS_INITIALIZERS(A, B, C) {#A, B, C},

// Defines a native enum corresponding to the enum values.
#define ENUM_DEFINE_CPP_ENUM(Y)                                                                                        \
    enum class Value { Y(ENUM_SELECT_FIRST_ARGUMENT) };                                                                \
    Value getValueFromIdentifier(babelwires::FieldIdentifier id) const {                                               \
        const EnumValues& values = getEnumValues();                                                                    \
        const auto it = std::find(values.begin(), values.end(), id);                                                   \
        assert((it != values.end()) && "id not found in enum");                                                        \
        return static_cast<Value>(it - values.begin());                                                                \
    }                                                                                                                  \
    babelwires::FieldIdentifier getIdentifierFromValue(Value value) {                                                  \
        const EnumValues& values = getEnumValues();                                                                    \
        return values[static_cast<unsigned int>(value)];                                                               \
    }


namespace babelwires {

    /// Defines the set of values available for enum features.
    /// TODO: Allow the user to create maps between Integers and Enums or Enums and Enums.
    class Enum : public RegistryEntry {
      public:
        /// The set of values. We use FieldIdentifiers to get versionable serialization support.
        /// Note: This matches the result of FIELD_NAME_VECTOR.
        // TODO Consider making the name "FieldIdentifier" less field specific now.
        using EnumValues = std::vector<FieldIdentifier>;

        Enum(std::string identifier, std::string name, VersionNumber version, const EnumValues& values,
             unsigned int indexOfDefaultValue);

        /// Get the set of available enum values.
        const EnumValues& getEnumValues() const;

        /// Get the index of the default value in the vector of enum values.
        unsigned int getIndexOfDefaultValue() const;

      private:
        const EnumValues& m_values;
        unsigned int m_indexOfDefaultValue;
    };

    /// A static way of finding the registered instance.
    template <typename E> class RegisteredEnum : public Enum {
      public:
        RegisteredEnum(std::string identifier, std::string name, VersionNumber version, const EnumValues& values,
                       unsigned int indexOfDefaultValue)
            : Enum(std::move(identifier), std::move(name), version, values, indexOfDefaultValue) {}

        static const E* getRegisteredInstance() { return s_registeredInstance; }

        void onRegistered() override {
            assert((s_registeredInstance == nullptr) && "Only one entry of each enum type can be registered");
            s_registeredInstance = static_cast<E*>(this);
        }

      private:
        static const E* s_registeredInstance;
    };

    template<typename E> const E* RegisteredEnum<E>::s_registeredInstance = nullptr;

    class EnumRegistry : public Registry<Enum> {
      public:
        EnumRegistry();
    };
} // namespace babelwires
