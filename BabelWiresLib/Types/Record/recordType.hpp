/**
 * RecordTypes are compound types containing a sequence of named children.
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

    /// RecordTypes are compound types containing a sequence of named children.
    class RecordType : public CompoundType {
      public:
        struct Field {
            ShortId m_identifier;
            TypeRef m_type;
        };

        /// An initialSize of -1 means the initial size is the minimum size.
        RecordType(std::vector<Field> fields);

        std::string getKind() const override;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

      public:
        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue, unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue, unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;

      private:
        std::vector<Field> m_fields;
    };

/* For testing
    // TODO Remove
    class TestRecordType : public RecordType {
      public:
        TestRecordType();
        
        PRIMITIVE_TYPE("arrayT", "Array", "0171a8a6-9ff7-4fed-a7fe-836529b690ae", 1);
    };

*/
} // namespace babelwires
