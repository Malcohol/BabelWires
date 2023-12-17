/**
 * RecordValues are compound values containing a set of named children.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <vector>

namespace babelwires {
    /// RecordValues are compound values containing a set of named children.
    class RecordValue : public Value {
      public:
        CLONEABLE(RecordValue);

        ValueHolder& getValue(ShortId fieldId);
        const ValueHolder& getValue(ShortId fieldId) const;

        ValueHolder* tryGetValue(ShortId fieldId);
        const ValueHolder* tryGetValue(ShortId fieldId) const;

        void setValue(ShortId fieldId, ValueHolder newValue);
        void removeValue(ShortId fieldId);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;

      private:
        std::unordered_map<ShortId, ValueHolder> m_fieldValues;
    };

} // namespace babelwires
