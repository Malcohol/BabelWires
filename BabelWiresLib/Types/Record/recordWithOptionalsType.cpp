/**
 * RecordWithOptionalsType is like a RecordType but some fields can be inactive.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordWithOptionalsType.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>

babelwires::RecordWithOptionalsType::RecordWithOptionalsType(std::vector<FieldWithOptionality> fields)
    : m_fields(std::move(fields)) {
    for (const auto& f : m_fields) {
        if (f.m_optionality != Optionality::alwaysActive) {
            m_optionalFieldIds.emplace_back(f.m_identifier);
        }
    }
}

const babelwires::RecordWithOptionalsType::FieldWithOptionality&
babelwires::RecordWithOptionalsType::getField(ShortId fieldId) const {
    for (const auto& f : m_fields) {
        if (f.m_identifier == fieldId) {
            return f;
        }
    }
    throw ModelException() << "Field " << fieldId << " not found in " << getTypeRef();
}

void babelwires::RecordWithOptionalsType::activateField(const TypeSystem& typeSystem, ValueHolder& value,
                                                        ShortId fieldId) const {
    const FieldWithOptionality& field = getField(fieldId);

    if (field.m_optionality == Optionality::alwaysActive) {
        throw ModelException() << "Field " << fieldId << " is not an optional and cannot be activated";
    }

    RecordValue& recordValue = value.copyContentsAndGetNonConst().is<RecordValue>();
    const Type& fieldType = field.m_type.resolve(typeSystem);
    // If the value happens to be already there, it gets overridden.
    recordValue.setValue(field.m_identifier, fieldType.createValue(typeSystem));
}

void babelwires::RecordWithOptionalsType::deactivateField(ValueHolder& value, ShortId fieldId) const {
    const FieldWithOptionality& field = getField(fieldId);

    if (field.m_optionality == Optionality::alwaysActive) {
        throw ModelException() << "Field " << fieldId << " is not an optional and cannot be deactivated";
    }

    RecordValue& recordValue = value.copyContentsAndGetNonConst().is<RecordValue>();
    recordValue.removeValue(field.m_identifier);
}

bool babelwires::RecordWithOptionalsType::isOptional(ShortId fieldId) const {
    return std::find(m_optionalFieldIds.begin(), m_optionalFieldIds.end(), fieldId) != m_optionalFieldIds.end();
}

bool babelwires::RecordWithOptionalsType::isActivated(const ValueHolder& value, ShortId fieldId) const {
    const RecordValue& recordValue = value->is<RecordValue>();
    return recordValue.tryGetValue(fieldId);
}

const std::vector<babelwires::ShortId>& babelwires::RecordWithOptionalsType::getOptionalFieldIds() const {
    return m_optionalFieldIds;
}

unsigned int babelwires::RecordWithOptionalsType::getNumActiveFields(const ValueHolder& value) const {
    const RecordValue& recordValue = value->is<RecordValue>();
    unsigned int numActiveFields = 0;
    for (const auto& f : m_optionalFieldIds) {
        if (recordValue.tryGetValue(f)) {
            ++numActiveFields;
        }
    }
    return numActiveFields;
}

babelwires::NewValueHolder babelwires::RecordWithOptionalsType::createValue(const TypeSystem& typeSystem) const {
    auto newValue = babelwires::ValueHolder::makeValue<RecordValue>();
    for (const auto& f : m_fields) {
        if (f.m_optionality != Optionality::optionalDefaultInactive) {
            const Type& fieldType = f.m_type.resolve(typeSystem);
            newValue.m_nonConstReference.is<RecordValue>().setValue(f.m_identifier, fieldType.createValue(typeSystem));
        }
    }
    return newValue;
}

bool babelwires::RecordWithOptionalsType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const RecordValue* const recordValue = v.as<RecordValue>();
    if (!recordValue) {
        return false;
    }
    for (const auto& f : m_fields) {
        const ValueHolder* const value = recordValue->tryGetValue(f.m_identifier);
        if (!value) {
            if (f.m_optionality == Optionality::alwaysActive) {
                return false;
            }
        } else {
            const Type& fieldType = f.m_type.resolve(typeSystem);
            if (!fieldType.isValidValue(typeSystem, **value)) {
                return false;
            }
        }
    }
    // Allow extra fields ("duck typing")
    return true;
}

unsigned int babelwires::RecordWithOptionalsType::getNumChildren(const ValueHolder& compoundValue) const {
    // Although the value might have additional children, it is being queried here through the lens of _this_ type.
    return m_fields.size() - m_optionalFieldIds.size() + getNumActiveFields(compoundValue);
}

const babelwires::RecordWithOptionalsType::FieldWithOptionality&
babelwires::RecordWithOptionalsType::getFieldFromChildIndex(const ValueHolder& compoundValue,
                                                                 unsigned int i) const {
    unsigned int j = 0;
    for (const auto& f : m_fields) {
        if ((f.m_optionality == Optionality::alwaysActive) || isActivated(compoundValue, f.m_identifier)) {
            if (i == j) {
                return f;
            }
            ++j;
        }
    }
    assert(false && "Child index out of range");
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordWithOptionalsType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    const FieldWithOptionality& f = getFieldFromChildIndex(compoundValue, i);
    const RecordValue& recordValue = compoundValue->is<RecordValue>();
    const ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordWithOptionalsType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    const FieldWithOptionality& f = getFieldFromChildIndex(compoundValue, i);
    RecordValue& recordValue = compoundValue.copyContentsAndGetNonConst().is<RecordValue>();
    ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

int babelwires::RecordWithOptionalsType::getChildIndexFromStep(const ValueHolder& compoundValue,
                                                               const PathStep& step) const {
    if (!step.isField()) {
        return -1;
    }
    const ShortId id = *step.asField();
    int index = 0;
    for (const auto& f : m_fields) {
        if ((f.m_optionality == Optionality::alwaysActive) || isActivated(compoundValue, f.m_identifier)) {
            if (f.m_identifier == id) {
                return index;
            }
            ++index;
        } else if (f.m_identifier == id) {
            return -1;
        }
    }
    return -1;
}
