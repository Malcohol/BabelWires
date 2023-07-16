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

        const Type& getType() const;

        void updateSubfeatures() const;

      protected:
        virtual void doSetToDefault() override;

        virtual std::size_t doGetHash() const override;

        const TypeRef& doGetTypeRef() const override;
        ValueHolder& doGetValue() override;
        const ValueHolder& doGetValue() const override;
        void doSetValue(const ValueHolder& newValue) override;

      private:
        TypeRef m_typeRef;
        ValueHolder m_value;
    };
} // namespace babelwires
