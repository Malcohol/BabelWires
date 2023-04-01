/**
 * A ValueFeature is a feature which stores a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;

    /// A ValueFeature is a feature which stores a value.
    class SimpleValueFeature : public ValueFeature {
      public:
        SimpleValueFeature(TypeRef typeRef);

        const Type& getType() const;

        const Value& getValue() const;
        
        void setValue(const Value& newValue);
        void setValue(Value&& newValue);

        void setValueHolder(const ValueHolder& newValue);

      protected:
        virtual std::string doGetValueType() const override;

        virtual void doAssign(const ValueFeature& other) override;

        virtual void doSetToDefault() override;

        virtual std::size_t doGetHash() const override;
      private:
        TypeRef m_typeRef;
        ValueHolder m_value;
    };
} // namespace babelwires
