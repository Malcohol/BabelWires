/**
 * TestArrayType
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Domains/TestDomain/testDomainExport.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

namespace testDomain {
    class TESTDOMAIN_API TestSimpleArrayType : public babelwires::ArrayType {
      public:
        DOWNCASTABLE(TestSimpleArrayType, babelwires::ArrayType);

        TestSimpleArrayType(const babelwires::TypeSystem& typeSystem);

        static constexpr unsigned int s_minimumSize = 0;
        static constexpr unsigned int s_maximumSize = 10;
        static constexpr unsigned int s_defaultSize = 4;
        static constexpr unsigned int s_nonDefaultSize = 6;

        static babelwires::TypeExp getEntryTypeStatic();

        REGISTERED_TYPE("sarray", "Test Simple Array", "2bca3f50-8d9d-46ba-bf43-e0a583f5bcf7", 1);
    };

    class TESTDOMAIN_API TestCompoundArrayType : public babelwires::ArrayType {
      public:
        DOWNCASTABLE(TestCompoundArrayType, babelwires::ArrayType);

        TestCompoundArrayType(const babelwires::TypeSystem& typeSystem);

        static constexpr unsigned int s_minimumSize = 2;
        static constexpr unsigned int s_maximumSize = 4;
        static constexpr unsigned int s_defaultSize = 3;

        static babelwires::TypeExp getEntryTypeStatic();

        REGISTERED_TYPE("carray", "Test Compound Array", "e2125eb9-b8a5-470a-a287-7142ff5a6b9c", 1);
    };

    /// ValueNodeData which creates an element carrying a simple array.
    class TESTDOMAIN_API TestArrayElementData : public babelwires::ValueNodeData {
      public:
        DOWNCASTABLE(TestArrayElementData, babelwires::ValueNodeData);
        TestArrayElementData();

        static babelwires::Path getPathToArray();
        static babelwires::Path getPathToArrayEntry(unsigned int i);
        static babelwires::Path getPathToArray_1();
        static babelwires::Path getPathToArray_2();
    };

} // namespace testDomain
