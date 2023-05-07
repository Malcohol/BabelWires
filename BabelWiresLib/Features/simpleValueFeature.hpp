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
        /// Prefer the constructor which takes a TypeRef argument.
        /// This allows the TypeRef to be provided at a slightly later time. However, it must be set before
        /// doSetToDefault is called. (Setting it in a subclass override of doSetToDefault is OK.)
        SimpleValueFeature();

        /// Construct a ValueFeature which carries values of the given type.
        SimpleValueFeature(TypeRef typeRef);

        const TypeRef& getTypeRef() const;

        const Type& getType() const;

        const Value& getValue() const;

        void setValue(const Value& newValue);
        void setValue(Value&& newValue);

        void setValueHolder(const ValueHolder& newValue);

      protected:
        /// This just exists so subclasses can set the type the first time if they use the default constructor.
        /// It asserts if the TypeRef has already been set.
        void setTypeRef(TypeRef type);

        virtual std::string doGetValueType() const override;

        virtual void doAssign(const ValueFeature& other) override;

        virtual void doSetToDefault() override;

        virtual std::size_t doGetHash() const override;

      private:
        TypeRef m_typeRef;
        ValueHolder m_value;
    };
} // namespace babelwires
