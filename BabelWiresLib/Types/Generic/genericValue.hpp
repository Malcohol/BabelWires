/**
 * A GenericValue holds the type assignment for a GenericType, and an instantiation of its wrapped type.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/Path/path.hpp>

namespace babelwires {
    class GenericValue : public Value {
      public:
        CLONEABLE(GenericValue);

        GenericValue(const TypeSystem& typeSystem, TypeExp wrappedType, unsigned int numVariables);

        /// This may have instantiations
        const TypeExp& getActualWrappedType() const;

        const std::vector<TypeExp>& getTypeAssignments() const;
        std::vector<TypeExp>& getTypeAssignments();

        /// Update the typeVariableAssignments
        void instantiate(const TypeSystem& typeSystem, const TypeExp& wrappedTypeRef);

        const ValueHolder& getValue() const;
        ValueHolder& getValue();

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;

        /// Check whether the actual wrapped type is a version of the given type with
        bool isActualVersionOf(const TypeExp& wrappedType) const;

      private:
        /// Build a version of wrappedType where type variables are substituted according to the
        /// type assignment.
        TypeExp buildInstantiatedType(const TypeExp& wrappedType) const;
        
      private:
        /// The actual type may differ from the GenericType's wrapped type
        /// because variables will have been substituted.
        TypeExp m_actualWrappedType;
        /// When unassigned, these hold a default constructed TypeExp.
        std::vector<TypeExp> m_typeVariableAssignments;
        /// The current value of the actualWrappedType.
        ValueHolder m_wrappedValue;
    };
}
