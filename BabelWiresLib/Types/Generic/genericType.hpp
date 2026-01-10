/**
 * GenericType is a type that wraps another type containing type variables that can be instantiated in different ways.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace babelwires {

    /// Generic type (aka Parameterized Polymorphic Type, aka Template in C++) are types that wrap another type
    /// containing type variables that can be instantiated in different ways.
    /// The type always has a single child, which is the type it wraps.
    class GenericType : public CompoundType {
      public:
        /// Create a generic type with the given number of variables.
        GenericType(TypeExp typeExp, const TypeSystem& typeSystem, const TypeExp& wrappedType,
                    unsigned int numVariables);

        GenericType(TypeExp typeExp, const TypePtr& wrappedType, unsigned int numVariables);

        std::string getFlavour() const override;

        /// The PathStep from the GenericType to the child it wraps.
        static babelwires::ShortId getStepToValue();

        /// Instantiate the type variables using the given type type assignment.
        void setTypeVariableAssignmentAndInstantiate(const TypeSystem& typeSystem, ValueHolder& genericValue,
                                                     const std::vector<TypeExp>& typeVariableAssignments) const;

        /// Get the number of type variables in this GenericType.
        unsigned int getNumVariables() const;

        /// Get the type assigned to the given type variable.
        const TypeExp& getTypeAssignment(const ValueHolder& genericValue, unsigned int variableIndex) const;

        /// Are any of the type variables unassigned?
        bool isAnyTypeVariableUnassigned(const ValueHolder& genericValue) const;

        /// A guarantee that there's always a single child, which is the wrapped type.
        constexpr static unsigned int c_numChildren = 1;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypePtr&> getChild(const ValueHolder& compoundValue,
                                                                          unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypePtr&> getChildNonConst(ValueHolder& compoundValue,
                            unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem,
                                                         const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        TypePtr m_wrappedType;
        unsigned int m_numVariables;
        // The maximum number of nested generic types in the wrapped type.
        unsigned int m_genericTypeHeight;
    };
} // namespace babelwires
