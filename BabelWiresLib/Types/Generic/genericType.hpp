/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace babelwires {

    /// A type that wraps another type containing type variables that can be instantiated in
    /// different ways.
    /// Type type always has a single child, which is the type it wraps.
    class GenericType : public CompoundType {
      public:
        /// Create a generic type with the given number of variables.
        /// A default constructed TypeRef means no restriction.
        GenericType(TypeRef wrappedType, unsigned int numVariables);

        std::string getFlavour() const override;

        static babelwires::ShortId getStepToValue();

        /// Instantiate the specified type variable using the given type.
        void instantiate(ValueHolder& genericValue, unsigned int variableIndex, const TypeRef& typeValue) const;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue,
                                                                          unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue,
                                                                            unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        TypeRef m_wrappedType;
        unsigned int m_numVariables;
    };
} // namespace babelwires
