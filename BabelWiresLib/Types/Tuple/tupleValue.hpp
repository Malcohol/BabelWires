/**
 * TupleValues are instances of TupleTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

#include <vector>

namespace babelwires {
    /// RecordValues are compound values containing a set of named children.
    class TupleValue : public Value {
      public:
        CLONEABLE(TupleValue);

        using Tuple = std::vector<ValueHolder>;

        TupleValue(Tuple values);

        unsigned int getSize() const;
        ValueHolder& getValue(unsigned int index);
        const ValueHolder& getValue(unsigned int index) const;
        void setValue(unsigned int index, ValueHolder newValue);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;

      private:
        std::vector<ValueHolder> m_values;
    };

} // namespace babelwires
