/**
 * RecordType is like a RecordType but some fields can be inactive.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>

// TODO Remove
#include <BabelWiresLib/Types/Int/intType.hpp>

/*
babelwires::TestRecordType::TestRecordType()
    : RecordType(
          {{BW_SHORT_ID("foo", "Foo", "b36ab40f-c570-46f7-9dab-3af1b8f3216e"), DefaultIntType::getThisIdentifier()},
           {BW_SHORT_ID("erm", "Erm", "bcb21539-6d10-41b2-886b-4b46f158f6bd"), DefaultIntType::getThisIdentifier()}}) {}

babelwires::TestRecordType2::TestRecordType2()
    : RecordType(
          {{BW_SHORT_ID("bar", "Bar", "8e746efa-1db8-4c43-b80c-451b6ee5db55"), DefaultIntType::getThisIdentifier()},
           {BW_SHORT_ID("obj", "Obj", "e4629b27-0286-4712-8cf4-6cce69bb3636"), TestRecordType::getThisIdentifier()},
           {BW_SHORT_ID("merm", "Merm", "b69f16a6-fcfb-49e8-be4b-c7910bff15c7"), DefaultIntType::getThisIdentifier(),
            Optionality::optionalDefaultInactive}}) {}
*/

babelwires::RecordType::RecordType(std::vector<Field> fields)
    : m_fields(std::move(fields)) {
    for (const auto& f : m_fields) {
        if (f.m_optionality != Optionality::alwaysActive) {
            m_optionalFieldIds.emplace_back(f.m_identifier);
        }
    }
}

std::string babelwires::RecordType::getKind() const {
    return "record";
}

const babelwires::RecordType::Field& babelwires::RecordType::getField(ShortId fieldId) const {
    for (const auto& f : m_fields) {
        if (f.m_identifier == fieldId) {
            return f;
        }
    }
    throw ModelException() << "Field " << fieldId << " not found in " << getTypeRef();
}

void babelwires::RecordType::activateField(const TypeSystem& typeSystem, ValueHolder& value, ShortId fieldId) const {
    const Field& field = getField(fieldId);

    if (field.m_optionality == Optionality::alwaysActive) {
        throw ModelException() << "Field " << fieldId << " is not an optional and cannot be activated";
    } 
    if (isActivated(value, fieldId)) {
        throw ModelException() << "Field " << fieldId << " is already activated";
    }

    RecordValue& recordValue = value.copyContentsAndGetNonConst().is<RecordValue>();
    const Type& fieldType = field.m_type.resolve(typeSystem);
    // If the value happens to be already there, it gets overridden.
    recordValue.setValue(field.m_identifier, fieldType.createValue(typeSystem));
}

void babelwires::RecordType::deactivateField(ValueHolder& value, ShortId fieldId) const {
    const Field& field = getField(fieldId);

    if (field.m_optionality == Optionality::alwaysActive) {
        throw ModelException() << "Field " << fieldId << " is not an optional and cannot be deactivated";
    }
    if (!isActivated(value, fieldId)) {
        throw ModelException() << "Field " << fieldId << " is not activated";
    }

    RecordValue& recordValue = value.copyContentsAndGetNonConst().is<RecordValue>();
    recordValue.removeValue(field.m_identifier);
}

void babelwires::RecordType::ensureActivated(const TypeSystem& typeSystem, ValueHolder& value,
                                             const std::vector<ShortId>& optionalsToEnsureActivated) const {
    // Avoid modifying the value if we throw after partially processing the fields.
    ValueHolder temp = value;
    RecordValue& recordValue = temp.copyContentsAndGetNonConst().is<RecordValue>();

    std::vector<ShortId> availableOptionals = getOptionalFieldIds();
    std::sort(availableOptionals.begin(), availableOptionals.end());
    auto ait = availableOptionals.begin();

    std::vector<ShortId> ensureActivated = optionalsToEnsureActivated;
    std::sort(ensureActivated.begin(), ensureActivated.end());
    auto it = ensureActivated.begin();

    std::vector<ShortId> missingOptionals;

    while ((ait != availableOptionals.end()) && (it != ensureActivated.end())) {
        if (*ait == *it) {
            if (!recordValue.tryGetValue(*ait)) {
                const Field& field = getField(*ait);
                const Type& fieldType = field.m_type.resolve(typeSystem);
                recordValue.setValue(*ait, fieldType.createValue(typeSystem));
            }
            ++ait;
            ++it;
        } else if (*ait < *it) {
            if (recordValue.tryGetValue(*ait)) {
                recordValue.removeValue(*ait);
            }
            ++ait;
        } else {
            missingOptionals.emplace_back(*it);
            ++it;
        }
    }
    while (ait != availableOptionals.end()) {
        if (recordValue.tryGetValue(*ait)) {
            recordValue.removeValue(*ait);
        }
        ++ait;
    }
    while (it != ensureActivated.end()) {
        missingOptionals.emplace_back(*it);
        ++it;
    }
    if (!missingOptionals.empty()) {
        throw ModelException() << "Not all of the optionals could be activated";
    }
    value = temp;
}

bool babelwires::RecordType::isOptional(ShortId fieldId) const {
    return std::find(m_optionalFieldIds.begin(), m_optionalFieldIds.end(), fieldId) != m_optionalFieldIds.end();
}

bool babelwires::RecordType::isActivated(const ValueHolder& value, ShortId fieldId) const {
    const RecordValue& recordValue = value->is<RecordValue>();
    return recordValue.tryGetValue(fieldId);
}

const std::vector<babelwires::RecordType::Field>& babelwires::RecordType::getFields() const {
    return m_fields;
}

const std::vector<babelwires::ShortId>& babelwires::RecordType::getOptionalFieldIds() const {
    return m_optionalFieldIds;
}

unsigned int babelwires::RecordType::getNumActiveFields(const ValueHolder& value) const {
    const RecordValue& recordValue = value->is<RecordValue>();
    unsigned int numActiveFields = 0;
    for (const auto& f : m_optionalFieldIds) {
        if (recordValue.tryGetValue(f)) {
            ++numActiveFields;
        }
    }
    return numActiveFields;
}

babelwires::NewValueHolder babelwires::RecordType::createValue(const TypeSystem& typeSystem) const {
    auto newValue = babelwires::ValueHolder::makeValue<RecordValue>();
    for (const auto& f : m_fields) {
        if (f.m_optionality != Optionality::optionalDefaultInactive) {
            const Type& fieldType = f.m_type.resolve(typeSystem);
            newValue.m_nonConstReference.is<RecordValue>().setValue(f.m_identifier, fieldType.createValue(typeSystem));
        }
    }
    return newValue;
}

bool babelwires::RecordType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
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

unsigned int babelwires::RecordType::getNumChildren(const ValueHolder& compoundValue) const {
    // Although the value might have additional children, it is being queried here through the lens of _this_ type.
    return m_fields.size() - m_optionalFieldIds.size() + getNumActiveFields(compoundValue);
}

const babelwires::RecordType::Field& babelwires::RecordType::getFieldFromChildIndex(const ValueHolder& compoundValue,
                                                                                    unsigned int i) const {
    unsigned int j = 0;
    if (m_optionalFieldIds.size() == 0) {
        return m_fields[i];
    } else {
        for (const auto& f : m_fields) {
            if ((f.m_optionality == Optionality::alwaysActive) || isActivated(compoundValue, f.m_identifier)) {
                if (i == j) {
                    return f;
                }
                ++j;
            }
        }
    }
    assert(false && "Child index out of range");
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    const Field& f = getFieldFromChildIndex(compoundValue, i);
    const RecordValue& recordValue = compoundValue->is<RecordValue>();
    const ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    const Field& f = getFieldFromChildIndex(compoundValue, i);
    RecordValue& recordValue = compoundValue.copyContentsAndGetNonConst().is<RecordValue>();
    ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

int babelwires::RecordType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
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

babelwires::SubtypeOrder babelwires::RecordType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                      const Type& other) const {
    const RecordType* const otherRecord = other.as<RecordType>();
    if (!otherRecord) {
        return SubtypeOrder::IsUnrelated;
    }
    SubtypeOrder currentOrder = SubtypeOrder::IsEquivalent;

    auto updateAndCheckUnrelated = [&currentOrder](SubtypeOrder localOrder) {
        currentOrder = subtypeOrderSupremum(currentOrder, localOrder);
        return (currentOrder == SubtypeOrder::IsUnrelated);
    };
    // Field order is not important for subtyping.
    // We use sorted sets of fields to allow mutual traversal by identifier.
    std::vector<Field> thisFields = m_fields;
    std::vector<Field> otherFields = otherRecord->m_fields;
    auto fieldLess = [](const Field& a, const Field& b) { return a.m_identifier < b.m_identifier; };
    std::sort(thisFields.begin(), thisFields.end(), fieldLess);
    std::sort(otherFields.begin(), otherFields.end(), fieldLess);
    auto thisIt = thisFields.begin();
    auto otherIt = otherFields.begin();
    while ((thisIt < thisFields.end()) && (otherIt < otherFields.end())) {
        if (thisIt->m_identifier == otherIt->m_identifier) {
            const Type* const thisFieldType = thisIt->m_type.tryResolve(typeSystem);
            const Type* const otherFieldType = otherIt->m_type.tryResolve(typeSystem);
            if (!thisFieldType || !otherFieldType) {
                return SubtypeOrder::IsUnrelated;
            }
            const SubtypeOrder fieldComparison = thisFieldType->compareSubtypeHelper(typeSystem, *otherFieldType);
            if (updateAndCheckUnrelated(fieldComparison)) {
                return SubtypeOrder::IsUnrelated;
            }
            ++thisIt;
            ++otherIt;
        } else if (thisIt->m_identifier < otherIt->m_identifier) {
            // A record with an additional _required_ field can be a subtype of one without it.
            // A record with an additional _optional_ field can actually have the same set of valid values as one
            // without it, since we use "duck typing".
            if ((thisIt->m_optionality == Optionality::alwaysActive) &&
                updateAndCheckUnrelated(SubtypeOrder::IsSubtype)) {
                return SubtypeOrder::IsUnrelated;
            }
            ++thisIt;
        } else { // if (otherIt->m_identifier < thisIt->m_identifier) {
            if ((otherIt->m_optionality == Optionality::alwaysActive) &&
                updateAndCheckUnrelated(SubtypeOrder::IsSupertype)) {
                return SubtypeOrder::IsUnrelated;
            }
            ++otherIt;
        }
    }
    if (thisIt != thisFields.end()) {
        if (updateAndCheckUnrelated(SubtypeOrder::IsSubtype)) {
            return SubtypeOrder::IsUnrelated;
        }
    } else if (otherIt != otherFields.end()) {
        if (updateAndCheckUnrelated(SubtypeOrder::IsSupertype)) {
            return SubtypeOrder::IsUnrelated;
        }
    }
    return currentOrder;
}

std::string babelwires::RecordType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const RecordValue& recordValue = v->is<RecordValue>();
    const unsigned int numActivatedFields = getNumActiveFields(v);
    const unsigned int numOptionalFields = m_optionalFieldIds.size();
    const unsigned int numChildren = m_fields.size() - numOptionalFields + numActivatedFields;
    std::ostringstream os;
    os << "{" << numChildren;
    if (numOptionalFields > 0) {
        os << " (" << numActivatedFields << "/" << numOptionalFields << " optional)";
    }
    os << "}";
    return os.str();
}
