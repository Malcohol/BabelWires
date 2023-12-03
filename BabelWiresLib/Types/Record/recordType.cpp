#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/Types/Record/recordValue.hpp>

babelwires::RecordType::RecordType(std::vector<Field> fields)
    : m_fields(std::move(fields)) {}

std::string babelwires::RecordType::getKind() const {
    return "record";
}

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
            // A record with additional fields can be a subtype of one with fewer.
            if (updateAndCheckUnrelated(SubtypeOrder::IsSubtype)) {
                return SubtypeOrder::IsUnrelated;
            }
            ++thisIt;
        } else if (otherIt->m_identifier < thisIt->m_identifier) {
            if (updateAndCheckUnrelated(SubtypeOrder::IsSupertype)) {
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
    return { &value, PathStep{f.m_identifier}, f.m_type };
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    RecordValue& recordValue = compoundValue.copyContentsAndGetNonConst().is<RecordValue>();
    const Field& f = m_fields[i];
    ValueHolder& value = recordValue.getValue(f.m_identifier);
    return { &value, PathStep{f.m_identifier}, f.m_type };
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
