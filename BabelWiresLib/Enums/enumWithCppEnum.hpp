/**
 * Convenience macros and base class for adding a corresponding C++ enum to a Enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Features/enumFeature.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

// Implementation detail.
#define ENUM_SELECT_FIRST_ARGUMENT(A, B, C) A,

/// Use in an Enum to add a C++ enum corresponding to the enum values.
/// Boiler-plate of the following kind is required:
/// #define MY_ENUM(X)                                                 \
///    X(Foo, "Foo value", "00000000-1111-2222-3333-444444444444")     \
///    X(Bar, "Bar value", "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee")
/// For coding convenience, NUM_VALUES and NotAValue entries are provided.
#define ENUM_DEFINE_CPP_ENUM(Y)                                                                                        \
    enum class Value { Y(ENUM_SELECT_FIRST_ARGUMENT) NUM_VALUES, NotAValue = NUM_VALUES };                             \
    Value getValueFromIdentifier(babelwires::Identifier id) const {                                                    \
        return static_cast<Value>(getIndexFromIdentifier(id));                                                         \
    }                                                                                                                  \
    babelwires::Identifier getIdentifierFromValue(Value value) const {                                                 \
        return getIdentifierFromIndex(static_cast<unsigned int>(value));                                               \
    }

// Implementation detail.
#define ENUM_ARGUMENTS_AS_INITIALIZERS(A, B, C) {#A, B, C},

/// This defines a static which should be placed in a .cpp where the Enum will be constructed.
#define ENUM_DEFINE_ENUM_VALUE_SOURCE(Y)                                                                               \
    namespace {                                                                                                        \
        const babelwires::IdentifiersSource s_enum_##Y = {Y(ENUM_ARGUMENTS_AS_INITIALIZERS)};                          \
    }

/// This should be used in the Enum constructor
#define ENUM_IDENTIFIER_VECTOR(Y) REGISTERED_ID_VECTOR(s_enum_##Y)

namespace babelwires {
    /// This provides convenience methods which allow the contained value of an EnumFeature
    /// to be queried or set using the corresponding C++ enum (assuming the latter was added to
    /// the Enum with ENUM_DEFINE_CPP_ENUM).
    template <typename E> class EnumWithCppEnumFeature : public RegisteredEnumFeature<E> {
      public:
        /// Get the stored value as a C++ enum value.
        typename E::Value getAsValue() const {
            return static_cast<const E&>(this->getEnum()).getValueFromIdentifier(this->m_value);
        }

        /// Set the value using a C++ enum value.
        void setFromValue(typename E::Value value) {
            set(static_cast<const E*>(this->getEnum())->getIdentifierFromValue(value));
        }
    };
} // namespace babelwires
