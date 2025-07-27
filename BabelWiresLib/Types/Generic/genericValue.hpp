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
#include <BabelWiresLib/Path/path.hpp>

namespace babelwires {
    class GenericValue : public Value {
      public:
        CLONEABLE(GenericValue);

        GenericValue(const TypeSystem& typeSystem, TypeRef wrappedType, unsigned int numVariables);

        const TypeRef& getWrappedType() const;

        const std::vector<TypeRef>& getTypeAssignments() const;

        /// Update the typeVariableAssignments
        void assignTypeVariableAndInstantiate(const TypeSystem& typeSystem, const TypeRef& wrappedTypeRef, unsigned int variableIndex, const TypeRef& typeValue);

        const ValueHolder& getValue() const;
        ValueHolder& getValue();

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
      private:
        /// Build a version of wrappedType where type variables are substituted according to the
        /// type assignment.
        TypeRef buildInstantiatedType(const TypeRef& wrappedType) const;
        
      private:
        /// The actual type may differ from the GenericType's wrapped type
        /// because variables will have been substituted.
        TypeRef m_actualWrappedType;
        /// When unassigned, these hold a default constructed TypeRef.
        std::vector<TypeRef> m_typeVariableAssignments;
        /// The current value of the actualWrappedType.
        ValueHolder m_wrappedValue;
    };
}
