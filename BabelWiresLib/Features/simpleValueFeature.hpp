/**
 * A SimpleValueFeature is a ValueFeature which owns its value.
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

    /// A SimpleValueFeature is a feature which owns its value.
    class SimpleValueFeature : public ValueFeature {
      public:
        /// Construct a ValueFeature which carries values of the given type.
        SimpleValueFeature(TypeRef typeRef);

        const TypeRef& getTypeRef() const override;
        const ValueHolder& getValue() const override;
        void setValue(const ValueHolder& newValue) override;

        const Type& getType() const;

      protected:
        virtual void doSetToDefault() override;

        virtual std::size_t doGetHash() const override;

      private:
        TypeRef m_typeRef;
        ValueHolder m_value;
    };
} // namespace babelwires
