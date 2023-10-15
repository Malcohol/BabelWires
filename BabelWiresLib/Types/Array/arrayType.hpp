/**
 * A type describes an array value.
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
        int getNumChildren(const Value& compoundValue) const override;
        const TypeRef& getChildType(const Value& compoundValue, unsigned int i) const override;
        ValueHolder* getChildNonConst(Value& compoundValue, unsigned int i) const override;
        const ValueHolder* getChild(const Value& compoundValue, unsigned int i) const override;
        PathStep getStepToChild(const Value& compoundValue, unsigned int i) const override;
        std::optional<std::tuple<TypeRef, ValueHolder&>> tryGetChildFromStepNonConst(ValueHolder& compoundValue, const PathStep& step) const override;
        std::optional<std::tuple<TypeRef, const ValueHolder&>> tryGetChildFromStep(const Value& compoundValue, const PathStep& step) const override;

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
        
        PRIMITIVE_TYPE("array", "Array", "0171a8a6-9ff7-4fed-a7fe-836529b690ae", 1);
    };

    class TestArrayType2 : public ArrayType {
      public:
        TestArrayType2();
        
        PRIMITIVE_TYPE("array2", "Array2", "0171a8a6-9ff7-4fed-a7fe-836529b690af", 1);
    };
} // namespace babelwires
