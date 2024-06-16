/**
 * Convenience macros and base class for adding a corresponding C++ enum to a Enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Enum/enumFeature.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

// Implementation details.
#define ENUM_SELECT_FIRST_ARGUMENT(A, B, C) A,
#define ENUM_ARGUMENTS_AS_INITIALIZERS(A, B, C) {#A, B, C},

#define ENUM_FEATURE_WRAPPER_METHODS(ENUM_TYPE)                                                                        \
    typename ENUM_TYPE::Value get() const {                                                                            \
        const babelwires::EnumValue& enumValue = m_valueFeature.getValue()->template is<babelwires::EnumValue>();      \
        const ENUM_TYPE& enumType = m_valueFeature.getType().template is<ENUM_TYPE>();                                 \
        return enumType.getValueFromIdentifier(enumValue.get());                                                       \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(typename ENUM_TYPE::Value newValue) {                \
        const ENUM_TYPE& enumType = m_valueFeature.getType().template is<ENUM_TYPE>();                                 \
        m_valueFeature.setValue(babelwires::EnumValue(enumType.getIdentifierFromValue(newValue)));                     \
    }

/// Use in an EnumType to add a C++ enum corresponding to the enum values.
/// Boiler-plate of the following kind is required:
/// #define MY_ENUM(X)                                                 \
///    X(Foo, "Foo value", "00000000-1111-2222-3333-444444444444")     \
///    X(Bar, "Bar value", "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee")
/// For coding convenience, NUM_VALUES and NotAValue entries are provided.
/// The EnumType constructor should use getStaticValueSet() to obtain its set of identifiers
#define ENUM_DEFINE_CPP_ENUM(Y)                                                                                        \
    enum class Value { Y(ENUM_SELECT_FIRST_ARGUMENT) NUM_VALUES, NotAValue = NUM_VALUES };                             \
    static const babelwires::IdentifiersSource s_enumValueInitializers;                                                \
    static const ValueSet& getStaticValueSet() {                                                                       \
        return REGISTERED_ID_VECTOR(s_enumValueInitializers);                                                          \
    }                                                                                                                  \
    static babelwires::ShortId getIdentifierFromValue(Value value) {                                                   \
        return getStaticValueSet()[static_cast<unsigned int>(value)];                                                  \
    }                                                                                                                  \
    Value getValueFromIdentifier(babelwires::ShortId id) const {                                                       \
        return static_cast<Value>(getIndexFromIdentifier(id));                                                         \
    }

/// This defines a static which should be placed in a .cpp where the Enum will be constructed.
#define ENUM_DEFINE_ENUM_VALUE_SOURCE(CLASS, Y)                                                                        \
    const babelwires::IdentifiersSource CLASS::s_enumValueInitializers = {Y(ENUM_ARGUMENTS_AS_INITIALIZERS)};

namespace babelwires {
    /// This provides convenience methods which allow the contained value of an EnumFeature
    /// to be queried or set using the corresponding C++ enum (assuming the latter was added to
    /// the Enum with ENUM_DEFINE_CPP_ENUM).
    template <typename E> class EnumWithCppEnumFeature : public EnumFeature {
      public:
        EnumWithCppEnumFeature()
            : EnumFeature(E::getThisIdentifier()) {}

        /// Get the stored value as a C++ enum value.
        typename E::Value getAsValue() const {
            return static_cast<const E&>(this->getEnum()).getValueFromIdentifier(this->get());
        }

        /// Set the value using a C++ enum value.
        void setFromValue(typename E::Value value) {
            this->set(static_cast<const E&>(this->getEnum()).getIdentifierFromValue(value));
        }
    };
} // namespace babelwires
