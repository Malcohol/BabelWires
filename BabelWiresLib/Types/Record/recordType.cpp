/**
 * RecordType is like a RecordType but some fields can be inactive.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

babelwires::RecordType::RecordType(std::vector<Field> fields)
    : m_fields(std::move(fields)) {
    for (const auto& f : m_fields) {
        if (f.m_optionality != Optionality::alwaysActive) {
            m_optionalFieldIds.emplace_back(f.m_identifier);
        }
    }
}

namespace {
    std::vector<babelwires::RecordType::Field>
    getCombinedFieldSet(const babelwires::RecordType& parent,
                        std::vector<babelwires::RecordType::Field> additionalFields) {
        const std::size_t numParentFields = parent.getFields().size();
        const std::size_t numAdditionalFields = additionalFields.size();
        const std::size_t combinedSize = numParentFields + numAdditionalFields;
        additionalFields.resize(combinedSize);
        std::move_backward(additionalFields.begin(), additionalFields.begin() + numAdditionalFields,
                           additionalFields.begin() + combinedSize);
        std::copy(parent.getFields().begin(), parent.getFields().end(), additionalFields.begin());
        return std::move(additionalFields);
    }
} // namespace

babelwires::RecordType::RecordType(const RecordType& parent, std::vector<Field> additionalFields)
    : RecordType(getCombinedFieldSet(parent, std::move(additionalFields))) {}

std::string babelwires::RecordType::getFlavour() const {
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
    return *static_cast<babelwires::RecordType::Field*>(0);
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

std::optional<babelwires::SubtypeOrder> babelwires::RecordType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                     const Type& other) const {
    const RecordType* const otherRecord = other.as<RecordType>();
    if (!otherRecord) {
        return {};
    }

    // Field order is not important for subtyping.
    // We use sorted sets of fields to allow mutual traversal by identifier.
    // MAYBEDO: Consider storing sorted fields in the record at construction.
    std::vector<Field> thisFields = m_fields;
    std::vector<Field> otherFields = otherRecord->m_fields;
    auto fieldLess = [](const Field& a, const Field& b) { return a.m_identifier < b.m_identifier; };
    std::sort(thisFields.begin(), thisFields.end(), fieldLess);
    std::sort(otherFields.begin(), otherFields.end(), fieldLess);

    SubtypeOrder containmentTest = SubtypeOrder::IsEquivalent;
    std::optional<SubtypeOrder> fixedSubTest;
    std::optional<SubtypeOrder> fixedSuperTest;

    auto thisIt = thisFields.begin();
    auto otherIt = otherFields.begin();
    while ((thisIt < thisFields.end()) && (otherIt < otherFields.end())) {
        if (thisIt->m_identifier == otherIt->m_identifier) {
            const SubtypeOrder fieldComparison = typeSystem.compareSubtype(thisIt->m_type, otherIt->m_type);
            containmentTest = subtypeProduct(containmentTest, fieldComparison);
            if (thisIt->m_optionality == Optionality::alwaysActive) {
                if (otherIt->m_optionality != Optionality::alwaysActive) {
                    containmentTest = subtypeProduct(containmentTest, SubtypeOrder::IsSubtype);
                }
                fixedSubTest = fixedSubTest ? subtypeProduct(*fixedSubTest, fieldComparison) : fieldComparison;
            }
            if (otherIt->m_optionality == Optionality::alwaysActive) {
                if (thisIt->m_optionality != Optionality::alwaysActive) {
                    containmentTest = subtypeProduct(containmentTest, SubtypeOrder::IsSupertype);
                }
                fixedSuperTest = fixedSuperTest ? subtypeProduct(*fixedSuperTest, fieldComparison) : fieldComparison;
            }
            ++thisIt;
            ++otherIt;
        } else if (thisIt->m_identifier < otherIt->m_identifier) {
            containmentTest = subtypeProduct(containmentTest, SubtypeOrder::IsSubtype);
            if (thisIt->m_optionality == Optionality::alwaysActive) {
                fixedSuperTest = SubtypeOrder::IsDisjoint;
            }
            ++thisIt;
        } else { // if (otherIt->m_identifier < thisIt->m_identifier) {
            containmentTest = subtypeProduct(containmentTest, SubtypeOrder::IsSupertype);
            if (otherIt->m_optionality == Optionality::alwaysActive) {
                fixedSubTest = SubtypeOrder::IsDisjoint;
            }
            ++otherIt;
        }
    }
    while (thisIt != thisFields.end()) {
        containmentTest = subtypeProduct(containmentTest, SubtypeOrder::IsSubtype);
        if (thisIt->m_optionality == Optionality::alwaysActive) {
            fixedSuperTest = SubtypeOrder::IsDisjoint;
        }
        ++thisIt;
    }
    while (otherIt != otherFields.end()) {
        containmentTest = subtypeProduct(containmentTest, SubtypeOrder::IsSupertype);
        if (otherIt->m_optionality == Optionality::alwaysActive) {
            fixedSubTest = SubtypeOrder::IsDisjoint;
        }
        ++otherIt;
    }
    if (containmentTest != SubtypeOrder::IsDisjoint) {
        return containmentTest;
    }
    if ((fixedSubTest && (fixedSubTest != SubtypeOrder::IsDisjoint)) || (fixedSuperTest && (fixedSuperTest != SubtypeOrder::IsDisjoint))) {
        return SubtypeOrder::IsIntersecting;
    }
    return SubtypeOrder::IsDisjoint;
}

std::string babelwires::RecordType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const RecordValue& recordValue = v->is<RecordValue>();
    const unsigned int numOptionalFields = m_optionalFieldIds.size();
    if (numOptionalFields > 0) {
        const unsigned int numActivatedFields = getNumActiveFields(v);
        std::ostringstream os;
        if (numOptionalFields > 0) {
            os << "Optionals: " << numActivatedFields << "/" << numOptionalFields;
        }
        return os.str();
    } else {
        return {};
    }
}
