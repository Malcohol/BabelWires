/**
 * RecordTypeBase is a common base of Types of RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordTypeBase.hpp>

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordWithOptionalsType.hpp>

std::string babelwires::RecordTypeBase::getKind() const {
    return "record";
}

babelwires::SubtypeOrder babelwires::RecordTypeBase::compareSubtypeHelper(const TypeSystem& typeSystem,
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
