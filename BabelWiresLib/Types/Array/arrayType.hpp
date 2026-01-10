/**
 * ArrayTypes have values which contain a dynamically-sized sequence of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace babelwires {

    /// ArrayTypes have values which contain a dynamically-sized sequence of child values.
    class ArrayType : public CompoundType {
      public:
        /// An initialSize of -1 means the initial size is the minimum size.
        ArrayType(TypeExp&& typeExpOfThis, TypePtr entryType, unsigned int minimumSize, unsigned int maximumSize,
                  int initialSize = -1);

        /// Get the valid size range of this array.
        Range<unsigned int> getSizeRange() const;

        /// Get the size of a newly initialized value of this array.
        unsigned int getInitialSize() const;

        TypeExp getEntryType() const;

        std::string getFlavour() const override;

        void setSize(const TypeSystem& typeSystem, ValueHolder& value, unsigned int newSize) const;
        void insertEntries(const TypeSystem& typeSystem, ValueHolder& value, unsigned int indexOfNewElement,
                           unsigned int numEntriesToAdd) const;
        void removeEntries(ValueHolder& value, unsigned int indexOfElementToRemove,
                           unsigned int numEntriesToRemove) const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem,
                                                         const Type& other) const override;

      public:
        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypePtr&> getChild(const ValueHolder& compoundValue,
                                                                          unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypePtr&> getChildNonConst(ValueHolder& compoundValue,
                            unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        TypePtr m_entryType;
        unsigned int m_initialSize;
        unsigned int m_minimumSize;
        unsigned int m_maximumSize;
    };
} // namespace babelwires
