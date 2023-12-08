/**
 * RecordTypes are compound types containing a sequence of named children.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/Types/Record/recordValue.hpp>

// TODO Remove
#include <BabelWiresLib/Types/Int/intType.hpp>

babelwires::TestRecordType::TestRecordType()
    : RecordType(
          {{BW_SHORT_ID("foo", "Foo", "b36ab40f-c570-46f7-9dab-3af1b8f3216e"), DefaultIntType::getThisIdentifier()},
          {BW_SHORT_ID("erm", "Erm", "bcb21539-6d10-41b2-886b-4b46f158f6bd"), DefaultIntType::getThisIdentifier()}}) {}

babelwires::TestRecordType2::TestRecordType2()
    : RecordType(
          {{BW_SHORT_ID("bar", "Bar", "8e746efa-1db8-4c43-b80c-451b6ee5db55"), DefaultIntType::getThisIdentifier()},
           {BW_SHORT_ID("obj", "Obj", "e4629b27-0286-4712-8cf4-6cce69bb3636"), TestRecordType::getThisIdentifier()}}) {}

babelwires::RecordType::RecordType(std::vector<Field> fields)
    : m_fields(std::move(fields)) {}

babelwires::NewValueHolder babelwires::RecordType::createValue(const TypeSystem& typeSystem) const {
    auto newValue = babelwires::ValueHolder::makeValue<RecordValue>();
    for (const auto& f : m_fields) {
        const Type& fieldType = f.m_type.resolve(typeSystem);
        newValue.m_nonConstReference.is<RecordValue>().setValue(f.m_identifier, fieldType.createValue(typeSystem));
    }
    return newValue;
}

bool babelwires::RecordType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    // Duck typing for records.
    const RecordValue* const recordValue = v.as<RecordValue>();
    if (!recordValue) {
        return false;
    }
    for (const auto& f : m_fields) {
        const ValueHolder* const value = recordValue->tryGetValue(f.m_identifier);
        if (!value) {
            return false;
        }
        const Type& fieldType = f.m_type.resolve(typeSystem);
        if (!fieldType.isValidValue(typeSystem, **value)) {
            return false;
        }
    }
    // Allow extra fields.
    return true;
}

unsigned int babelwires::RecordType::getNumChildren(const ValueHolder& compoundValue) const {
    assert(compoundValue->as<RecordValue>());
    // Although the value might have additional children, it is being queried here through the lens of _this_ type.
    return m_fields.size();
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    const RecordValue& recordValue = compoundValue->is<RecordValue>();
    const Field& f = m_fields[i];
    const ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    RecordValue& recordValue = compoundValue.copyContentsAndGetNonConst().is<RecordValue>();
    const Field& f = m_fields[i];
    ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

int babelwires::RecordType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
    if (!step.isField()) {
        return -1;
    }
    const ShortId id = *step.asField();
    for (unsigned int i = 0; i < m_fields.size(); ++i) {
        if (m_fields[i].m_identifier == id) {
            return i;
        }
    }
    return -1;
}
