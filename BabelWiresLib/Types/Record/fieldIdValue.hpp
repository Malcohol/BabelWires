/**
 * Holds the identifier of a field name.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/Detail/identifierValueBase.hpp>

namespace babelwires {

    class FieldIdValue : public IdentifierValueBase<ShortId> {
      public:
        CLONEABLE(FieldIdValue);
        SERIALIZABLE(FieldIdValue, "fieldId", EditableValue, 1);

        FieldIdValue();
        FieldIdValue(ShortId value);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
    };
} // namespace babelwires
