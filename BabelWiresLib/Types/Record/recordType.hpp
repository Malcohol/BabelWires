/**
 * RecordTypes are compound types containing a sequence of named children.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordTypeBase.hpp>

// TODO Remove
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    /// RecordTypes are compound types containing a sequence of named children.
    class RecordType : public RecordTypeBase {
      public:
        /// An initialSize of -1 means the initial size is the minimum size.
        RecordType(std::vector<Field> fields);

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

      public:
        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue, unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue, unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;

      private:
        friend RecordTypeBase;
        std::vector<Field> m_fields;
    };

    // TODO Remove
    class TestRecordType : public RecordType {
      public:
        TestRecordType();
        
        PRIMITIVE_TYPE("recordT", "Record", "7ce48078-c2e2-45c8-8c0e-dbad1f9fd771", 1);
    };

    class TestRecordType2 : public RecordType {
      public:
        TestRecordType2();
        
        PRIMITIVE_TYPE("recordT2", "Record2", "199e3fa7-5ddc-46c5-8eab-b66a121dac20", 1);
    };


} // namespace babelwires
