/**
 * ArrayTypes have values which contain a dynamically-sized sequence of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

// TODO Remove
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    /// ArrayTypes have values which contain a dynamically-sized sequence of child values.
    class ArrayType : public CompoundType {
      public:
        /// An initialSize of -1 means the initial size is the minimum size.
        ArrayType(TypeRef entryType, unsigned int minimumSize, unsigned int maximumSize, int initialSize = -1);

        const TypeRef& getEntryType() const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

      public:
        int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue, unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue, unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;

      private:
        TypeRef m_entryType;
        unsigned int m_initialSize;
        unsigned int m_minimumSize;
        unsigned int m_maximumSize;
    };

    // TODO Remove
    class TestArrayType : public ArrayType {
      public:
        TestArrayType();
        
        PRIMITIVE_TYPE("arrayT", "Array", "0171a8a6-9ff7-4fed-a7fe-836529b690ae", 1);
    };

    class TestArrayType2 : public ArrayType {
      public:
        TestArrayType2();
        
        PRIMITIVE_TYPE("arrayT2", "Array2", "0171a8a6-9ff7-4fed-a7fe-836529b690af", 1);
    };
} // namespace babelwires
