/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/compoundFeature.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/multiKeyMap.hpp>

namespace babelwires {
    class TypeRef;
    class Type;

    /// A ValueFeature is a feature which provides access to a value.
    class ValueFeature : public CompoundFeature {
      public:
        /// Construct a ValueFeature which carries values of the given type.
        ValueFeature(TypeRef typeRef);

        /// Get the TypeRef which describes the type of the value.
        const TypeRef& getTypeRef() const;

        /// Get the value currently held by this feature.
        const ValueHolder& getValue() const;

        /// Set this feature to hold a new value.
        void setValue(const ValueHolder& newValue);

        /// This is a convenience method which resolves the typeRef in the context of the TypeSystem
        /// carried by the rootFeature.
        const Type& getType() const;

        /// This is a convenience method which calls getType().getKind().
        /// The need for connectable features to provide a string description is not fundamental to the data model.
        /// It is imposed by the current UI.
        std::string getKind() const;

        /// Set this to hold the same value as other.
        /// This will throw a ModelException if the assignment failed.
        void assign(const ValueFeature& other);

      public:
        virtual int getNumFeatures() const override;
        virtual PathStep getStepToChild(const Feature* child) const override;
        virtual Feature* tryGetChildFromStep(const PathStep& step) override;
        virtual const Feature* tryGetChildFromStep(const PathStep& step) const override;
        virtual Feature* doGetFeature(int i) override;
        virtual const Feature* doGetFeature(int i) const override;
        /// Calls doSetToDefault.
        virtual void doSetToDefaultNonRecursive() override;
        virtual std::size_t doGetHash() const override;
        virtual void doSetToDefault() override;

      protected:
        virtual ValueHolder& doGetValue() = 0;
        virtual const ValueHolder& doGetValue() const = 0;

      private:
        TypeRef m_typeRef;
        MultiKeyMap<PathStep, unsigned int, std::unique_ptr<ValueFeature>> m_children;
    };
}
