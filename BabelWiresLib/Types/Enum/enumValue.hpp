/**
 * Holds a single value of an enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/Detail/identifierValueBase.hpp>

namespace babelwires {

    class EnumValue : public IdentifierValueBase<ShortId> {
      public:
        CLONEABLE(EnumValue);
        SERIALIZABLE(EnumValue, "enum", EditableValue, 1);

        EnumValue();
        EnumValue(ShortId value);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
    };
} // namespace babelwires
