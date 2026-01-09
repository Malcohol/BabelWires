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

namespace {
    void addFields(const babelwires::TypeSystem& typeSystem, std::vector<babelwires::RecordType::Field>& fields,
                   std::vector<babelwires::ShortId>& optionalFieldIds,
                   const std::vector<babelwires::RecordType::FieldDefinition>& newFields) {
        for (const auto& f : newFields) {
            fields.emplace_back(
                babelwires::RecordType::Field{f.m_identifier, f.m_type.resolve(typeSystem), f.m_optionality});
            assert(f.m_identifier.getDiscriminator() != 0 && "Field identifiers must be registered");
            if (f.m_optionality != babelwires::RecordType::Optionality::alwaysActive) {
                optionalFieldIds.emplace_back(f.m_identifier);
            }
        }
    }
} // namespace

babelwires::RecordType::RecordType(const TypeSystem& typeSystem, const std::vector<FieldDefinition>& fields) {
    m_fields.reserve(fields.size());
    addFields(typeSystem, m_fields, m_optionalFieldIds, fields);
}

babelwires::RecordType::RecordType(const TypeSystem& typeSystem, const RecordType& parent,
                                   const std::vector<FieldDefinition>& additionalFields) {
    m_fields.reserve(parent.getFields().size() + additionalFields.size());
    m_fields = parent.m_fields;
    m_optionalFieldIds = parent.m_optionalFieldIds;
    addFields(typeSystem, m_fields, m_optionalFieldIds, additionalFields);
}

babelwires::RecordType::RecordType(std::vector<Field> fields)
    : m_fields(std::move(fields)) {
    for (const auto& f : m_fields) {
        assert(f.m_identifier.getDiscriminator() != 0 && "Field identifiers must be registered");
        if (f.m_optionality != Optionality::alwaysActive) {
            m_optionalFieldIds.emplace_back(f.m_identifier);
        }
    }
}

std::string babelwires::RecordType::getFlavour() const {
    return "record";
}

const babelwires::RecordType::Field& babelwires::RecordType::getField(ShortId fieldId) const {
    for (const auto& f : m_fields) {
        if (f.m_identifier == fieldId) {
            return f;
        }
    }
    throw ModelException() << "Field " << fieldId << " not found in " << getTypeExp();
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
    // If the value happens to be already there, it gets overridden.
    recordValue.setValue(field.m_identifier, field.m_type->createValue(typeSystem));
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

void babelwires::RecordType::selectOptionals(const TypeSystem& typeSystem, ValueHolder& value,
                                             const std::map<ShortId, bool>& optionalsState) const {
    // Avoid modifying the value if we throw after partially processing the fields.
    ValueHolder temp = value;
    RecordValue& recordValue = temp.copyContentsAndGetNonConst().is<RecordValue>();

    unsigned int count = 0;
    std::vector<ShortId> availableOptionals = getOptionalFieldIds();
    for (const ShortId& fieldId : availableOptionals) {
        const Field& field = getField(fieldId);

        const auto it = optionalsState.find(fieldId);
        const bool isAssigned = (it != optionalsState.end());
        const bool isSetActivated = isAssigned && it->second;
        const bool isSetDeactivated = isAssigned && !it->second;
        if (isAssigned) {
            ++count;
        }
        assert(!(isSetActivated && isSetDeactivated) && "Field cannot be both activated and deactivated");
        const bool shouldBeActive =
            isSetActivated || (!isSetDeactivated && field.m_optionality == Optionality::optionalDefaultActive);
        const bool shouldBeInactive =
            isSetDeactivated || (!isSetActivated && field.m_optionality == Optionality::optionalDefaultInactive);
        if (shouldBeActive && !isActivated(temp, fieldId)) {
            recordValue.setValue(fieldId, field.m_type->createValue(typeSystem));
        } else if (shouldBeInactive && isActivated(temp, fieldId)) {
            recordValue.removeValue(fieldId);
        }
    }
    if (count < optionalsState.size()) {
        throw ModelException() << "Trying to set the state of an optional field which is not present in the record";
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
            newValue.m_nonConstReference.setValue(f.m_identifier, f.m_type->createValue(typeSystem));
        }
    }
    return std::move(newValue);
}

bool babelwires::RecordType::visitValue(const TypeSystem& typeSystem, const Value& v,
                                        ChildValueVisitor& visitor) const {
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
            if (!visitor(typeSystem, f.m_type->getTypeExp(), **value, PathStep{f.m_identifier})) {
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

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypePtr&>
babelwires::RecordType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    const Field& f = getFieldFromChildIndex(compoundValue, i);
    const RecordValue& recordValue = compoundValue->is<RecordValue>();
    const ValueHolder& value = recordValue.getValue(f.m_identifier);
    return {&value, PathStep{f.m_identifier}, f.m_type};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypePtr&>
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

    // This is suitable for testing containment, but is too strict to distinguish IsIntersecting from IsDisjoint.
    // For example, two records with incompatible optionals may still intersect.
    SubtypeOrder allFieldTest = SubtypeOrder::IsEquivalent;
    // These two orders are obtained solely from fixed fields and are intended to distinguish IsIntersecting from
    // IsDisjoint.
    std::optional<SubtypeOrder> fixedSubTest;
    std::optional<SubtypeOrder> fixedSuperTest;

    auto thisIt = thisFields.begin();
    auto otherIt = otherFields.begin();
    while ((thisIt < thisFields.end()) && (otherIt < otherFields.end())) {
        if (thisIt->m_identifier == otherIt->m_identifier) {
            const SubtypeOrder fieldComparison = typeSystem.compareSubtype(thisIt->m_type->getTypeExp(), otherIt->m_type->getTypeExp());
            allFieldTest = subtypeProduct(allFieldTest, fieldComparison);
            if (thisIt->m_optionality == Optionality::alwaysActive) {
                if (otherIt->m_optionality != Optionality::alwaysActive) {
                    allFieldTest = subtypeProduct(allFieldTest, SubtypeOrder::IsSubtype);
                }
                fixedSubTest = fixedSubTest ? subtypeProduct(*fixedSubTest, fieldComparison) : fieldComparison;
            }
            if (otherIt->m_optionality == Optionality::alwaysActive) {
                if (thisIt->m_optionality != Optionality::alwaysActive) {
                    allFieldTest = subtypeProduct(allFieldTest, SubtypeOrder::IsSupertype);
                }
                fixedSuperTest = fixedSuperTest ? subtypeProduct(*fixedSuperTest, fieldComparison) : fieldComparison;
            }
            ++thisIt;
            ++otherIt;
        } else if (thisIt->m_identifier < otherIt->m_identifier) {
            allFieldTest = subtypeProduct(allFieldTest, SubtypeOrder::IsSubtype);
            if (thisIt->m_optionality == Optionality::alwaysActive) {
                fixedSuperTest = SubtypeOrder::IsDisjoint;
            }
            ++thisIt;
        } else { // if (otherIt->m_identifier < thisIt->m_identifier) {
            allFieldTest = subtypeProduct(allFieldTest, SubtypeOrder::IsSupertype);
            if (otherIt->m_optionality == Optionality::alwaysActive) {
                fixedSubTest = SubtypeOrder::IsDisjoint;
            }
            ++otherIt;
        }
    }
    while (thisIt != thisFields.end()) {
        allFieldTest = subtypeProduct(allFieldTest, SubtypeOrder::IsSubtype);
        if (thisIt->m_optionality == Optionality::alwaysActive) {
            fixedSuperTest = SubtypeOrder::IsDisjoint;
        }
        ++thisIt;
    }
    while (otherIt != otherFields.end()) {
        allFieldTest = subtypeProduct(allFieldTest, SubtypeOrder::IsSupertype);
        if (otherIt->m_optionality == Optionality::alwaysActive) {
            fixedSubTest = SubtypeOrder::IsDisjoint;
        }
        ++otherIt;
    }
    if (allFieldTest != SubtypeOrder::IsDisjoint) {
        return allFieldTest;
    }
    if ((fixedSubTest && (fixedSubTest != SubtypeOrder::IsDisjoint)) ||
        (fixedSuperTest && (fixedSuperTest != SubtypeOrder::IsDisjoint))) {
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

std::tuple<const babelwires::ValueHolder&, const babelwires::TypePtr&>
babelwires::RecordType::getChildById(const ValueHolder& compoundValue, ShortId fieldId) const {
    assert(!isOptional(fieldId) || isActivated(compoundValue, fieldId));
    const RecordValue& recordValue = compoundValue->is<RecordValue>();
    const ValueHolder& fieldValue = recordValue.getValue(fieldId);
    const Field& field = getField(fieldId);
    return {fieldValue, field.m_type};
}

std::tuple<babelwires::ValueHolder&, const babelwires::TypePtr&>
babelwires::RecordType::getChildByIdNonConst(ValueHolder& compoundValue, ShortId fieldId) const {
    assert(!isOptional(fieldId) || isActivated(compoundValue, fieldId));
    RecordValue& recordValue = compoundValue.copyContentsAndGetNonConst().is<RecordValue>();
    ValueHolder& fieldValue = recordValue.getValue(fieldId);
    const Field& field = getField(fieldId);
    return {fieldValue, field.m_type};
}
