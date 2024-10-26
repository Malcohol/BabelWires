/**
 * A ChildValueFeature is a Feature whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;

    /// A ChildValueFeature is a Feature whose value is owned by an ancestor feature.
    class ChildValueFeature : public Feature {
      public:
        /// Construct a Feature which carries values of the given type.
        ChildValueFeature(TypeRef typeRef, const ValueHolder* valueHolder);

        /// If the new valueHolder pointer is different, update m_value and synchronizeSubfeatures.
        void ensureSynchronized(const ValueHolder* valueHolder);

      protected:
        const ValueHolder& doGetValue() const override;
        void doSetValue(const ValueHolder& newValue) override;
        void doSetToDefault() override;

      private:
        const ValueHolder* m_value;
    };
} // namespace babelwires
