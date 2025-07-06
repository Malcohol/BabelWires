/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    class GenericValue : public Value {
      public:
        CLONEABLE(GenericValue);

        GenericValue() = default;

        TypeRef getTypeRef() const;
        
        const ValueHolder& getValue() const;
        ValueHolder& getValue();

        void setTypeRef(const TypeSystem& typeSystem, TypeRef typeRef);
        void setValue(const TypeSystem& typeSystem, const ValueHolder& value);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;

      private:
        /// When this is not set, the
        TypeRef m_typeRef;
        ValueHolder m_value;
    };
}
