/**
 * An ArrayValue can contain a dynamically-sized sequence of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <vector>

namespace babelwires {
    /// An ArrayValue can contain a dynamically-sized sequence of child values.
    class ArrayValue : public Value {
      public:
        CLONEABLE(ArrayValue);
        ArrayValue(const TypeSystem& typeSystem, const Type& entryType, unsigned int initialSize);
        ArrayValue(const ArrayValue& other);
        ArrayValue(ArrayValue&& other);

        unsigned int getSize() const;
        void setSize(const TypeSystem& typeSystem, const Type& entryType, unsigned int newSize);

        ValueHolder& getValue(unsigned int index);
        const ValueHolder& getValue(unsigned int index) const;
        void setValue(unsigned int index, ValueHolder newValue);

        void insertValue(const TypeSystem& typeSystem, const Type& entryType, unsigned int index);
        void removeValue(unsigned int index);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;

      private:
        std::vector<ValueHolder> m_values;
    };

} // namespace babelwires
