/**
 * Converts an EnumValue to a native C++ value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

namespace babelwires {
    /// Convert an EnumValue to the identifier it carries.
    struct EnumToIdentifierValueAdapter {
        babelwires::ShortId operator() (const Value& value) const {
            const auto& enumValue = value.is<EnumValue>();
            return enumValue.get();
        }
    };

    /// Convert an EnumValue to a unsigned int corresponding to the index of the enum entry.
    /// Note: Be very careful with this if the enum could change in the future.
    struct EnumToIndexValueAdapter {
        TypePtrT<EnumType> m_enum;

        unsigned int operator() (const Value& value) const {
            const auto& enumValue = value.is<EnumValue>();
            return m_enum->getIndexFromIdentifier(enumValue.get());
        }
    };

    /// Convert an EnumValue to a native enum value.
    template<typename ENUM>
    struct EnumToValueValueAdapter {
        const ENUM& m_enum;

        typename ENUM::Value operator() (const Value& value) const {
            const auto& enumValue = value.is<EnumValue>();
            return m_enum.getValueFromIdentifier(enumValue.get());
        }
    };
} // namespace
