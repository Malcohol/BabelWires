/**
 * RecordWithVariantsType is like a RecordType but has a number of variants.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

// TODO Remove
#include <BabelWiresLib/Types/Int/intType.hpp>

namespace {
    babelwires::ShortId getTag0() {
        return BW_SHORT_ID("tag0", "Tag 0", "180b5292-5a1b-4849-8d0e-68444fc3554c");
    }

    babelwires::ShortId getTag1() {
        return BW_SHORT_ID("tag1", "Tag 1", "791ccc74-8fd8-4c29-a6f8-0349d9b189ad");
    }
} // namespace

babelwires::TestRecordWithVariants::TestRecordWithVariants()
    : RecordWithVariantsType(
          {getTag0(), getTag1()},
          {{BW_SHORT_ID("foo", "Foo", "b36ab40f-c570-46f7-9dab-3af1b8f3216e"),
            DefaultIntType::getThisIdentifier(),
            {getTag0()}},
           {BW_SHORT_ID("erm", "Erm", "bcb21539-6d10-41b2-886b-4b46f158f6bd"), DefaultIntType::getThisIdentifier()},
           {BW_SHORT_ID("oom", "Oom", "2cb79b61-c3b3-4df4-b8a0-11ec286bf659"),
            DefaultIntType::getThisIdentifier(),
            {getTag1()}}}) {}

babelwires::RecordWithVariantsType::RecordWithVariantsType(Tags tags, std::vector<FieldWithTags> fields,
                                                           unsigned int defaultTagIndex)
    : m_tags(std::move(tags))
    , m_defaultTag(m_tags[defaultTagIndex])
    , m_fields(std::move(fields)) {
    assert((m_tags.size() > 0) && "Empty tags set not allowed");
    assert(defaultTagIndex < m_tags.size());
    m_tagToVariantCache.reserve(m_tags.size());
    for (const auto& f : m_fields) {
        if (f.m_tags.empty()) {
            // Empty means this field is in every variant.
            for (const auto& t : m_tags) {
                m_tagToVariantCache[t].emplace_back(&f);
            }
        } else {
            for (const auto& t : f.m_tags) {
                const auto it = std::find(m_tags.begin(), m_tags.end(), t);
                assert((it != m_tags.end()) && "Field uses tag which is not in the tag set.");
                // m_fields is private and there is no API for manipulating it after construction, so this should be
                // safe.
                m_tagToVariantCache[t].emplace_back(&f);
            }
        }
    }
}

std::string babelwires::RecordWithVariantsType::getKind() const {
    return "variant";
}

const babelwires::RecordWithVariantsType::Tags& babelwires::RecordWithVariantsType::getTags() const {
    return m_tags;
}

unsigned int babelwires::RecordWithVariantsType::getIndexOfTag(ShortId tag) const {
    const auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it == m_tags.end()) {
        throw ModelException() << "The tag " << tag << " is not a tag of the type " << getTypeRef();
    }
    return std::distance(m_tags.begin(), it);
}

bool babelwires::RecordWithVariantsType::isTag(ShortId tag) const {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

void babelwires::RecordWithVariantsType::selectTag(const TypeSystem& typeSystem, ValueHolder& value, ShortId tag) const {
    const ShortId currentTag = getSelectedTag(value);
    const FieldChanges changes = getFieldChanges(currentTag, tag);
    RecordWithVariantsValue& recordValue = value.copyContentsAndGetNonConst().is<RecordWithVariantsValue>();
    for (auto i : changes.m_fieldsRemoved) {
        recordValue.removeValue(m_fields[i].m_identifier);
    }
    for (auto i : changes.m_fieldsAdded) {
        const Type& fieldType = m_fields[i].m_type.resolve(typeSystem);
        recordValue.setValue(m_fields[i].m_identifier, fieldType.createValue(typeSystem));
    }
    recordValue.setTag(tag);
}

babelwires::ShortId babelwires::RecordWithVariantsType::getSelectedTag(const ValueHolder& value) const {
    const auto& recordValue = value->is<RecordWithVariantsValue>();
    assert(isTag(recordValue.getTag()) && "RecordWithVariant has unrecognized tag");
    return recordValue.getTag();
}

babelwires::RecordWithVariantsType::FieldChanges
babelwires::RecordWithVariantsType::getFieldChanges(ShortId currentTag, ShortId proposedTag) const {
    FieldChanges fieldChanges;
    for (unsigned int i = 0; i < m_fields.size(); ++i) {
        const FieldWithTags& f = m_fields[i];
        const bool isCurrent = (std::find(f.m_tags.begin(), f.m_tags.end(), currentTag) != f.m_tags.end());
        const bool isProposed = (std::find(f.m_tags.begin(), f.m_tags.end(), proposedTag) != f.m_tags.end());
        if (isCurrent && !isProposed) {
            fieldChanges.m_fieldsRemoved.emplace_back(i);
        } else if (!isCurrent && isProposed) {
            fieldChanges.m_fieldsAdded.emplace_back(i);
        }
    }
    return fieldChanges;
}

std::vector<babelwires::ShortId>
babelwires::RecordWithVariantsType::getFieldsRemovedByChangeOfBranch(const ValueHolder& value,
                                                                     ShortId proposedTag) const {
    std::vector<babelwires::ShortId> fieldsRemoved;
    const FieldChanges fieldChanges = getFieldChanges(getSelectedTag(value), proposedTag);
    fieldsRemoved.reserve(fieldChanges.m_fieldsRemoved.size());
    for (auto i : fieldChanges.m_fieldsRemoved) {
        fieldsRemoved.emplace_back(m_fields[i].m_identifier);
    }
    return fieldsRemoved;
}

unsigned int babelwires::RecordWithVariantsType::getNumChildren(const ValueHolder& compoundValue) const {
    const ShortId tag = getSelectedTag(compoundValue);
    const auto it = m_tagToVariantCache.find(tag);
    assert(it != m_tagToVariantCache.end());
    return it->second.size();
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordWithVariantsType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    const ShortId tag = getSelectedTag(compoundValue);
    const auto it = m_tagToVariantCache.find(tag);
    assert(it != m_tagToVariantCache.end());
    const auto& recordValue = compoundValue->is<RecordWithVariantsValue>();
    const Field& f = *it->second[i];
    return {&recordValue.getValue(f.m_identifier), PathStep{f.m_identifier}, f.m_type};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::RecordWithVariantsType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    const ShortId tag = getSelectedTag(compoundValue);
    const auto it = m_tagToVariantCache.find(tag);
    assert(it != m_tagToVariantCache.end());
    RecordWithVariantsValue& recordValue = compoundValue.copyContentsAndGetNonConst().is<RecordWithVariantsValue>();
    const Field& f = *it->second[i];
    return {&recordValue.getValue(f.m_identifier), PathStep{f.m_identifier}, f.m_type};
}

int babelwires::RecordWithVariantsType::getChildIndexFromStep(const ValueHolder& compoundValue,
                                                              const PathStep& step) const {
    if (step.asIndex()) {
        return -1;
    }
    const ShortId stepId = *step.asField();
    const ShortId tag = getSelectedTag(compoundValue);
    const auto it = m_tagToVariantCache.find(tag);
    assert(it != m_tagToVariantCache.end());
    for (unsigned int i = 0; i < it->second.size(); ++i) {
        const Field& f = *it->second[i];
        if (f.m_identifier == stepId) {
            return i;
        }
    }
    return -1;
}

babelwires::NewValueHolder babelwires::RecordWithVariantsType::createValue(const TypeSystem& typeSystem) const {
    auto newValue = babelwires::ValueHolder::makeValue<RecordWithVariantsValue>(m_defaultTag);
    for (const auto* f : m_tagToVariantCache.find(m_defaultTag)->second) {
        const Type& fieldType = f->m_type.resolve(typeSystem);
        newValue.m_nonConstReference.is<RecordWithVariantsValue>().setValue(f->m_identifier,
                                                                            fieldType.createValue(typeSystem));
    }
    return newValue;
}

bool babelwires::RecordWithVariantsType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const RecordWithVariantsValue* const recordValue = v.as<RecordWithVariantsValue>();
    if (!recordValue) {
        return false;
    }
    const ShortId tag = recordValue->getTag();
    if (!isTag(tag)) {
        return false;
    }
    for (const auto* f : m_tagToVariantCache.find(tag)->second) {
        const ValueHolder* const value = recordValue->tryGetValue(f->m_identifier);
        if (!value) {
            return false;
        } else {
            const Type& fieldType = f->m_type.resolve(typeSystem);
            if (!fieldType.isValidValue(typeSystem, **value)) {
                return false;
            }
        }
    }
    return true;
}

std::string babelwires::RecordWithVariantsType::valueToString(const TypeSystem& typeSystem,
                                                              const ValueHolder& v) const {
    std::ostringstream os;
    const RecordWithVariantsValue* const recordValue = v->as<RecordWithVariantsValue>();
    auto identifierRegistry = IdentifierRegistry::read();
    os << "{" << getNumChildren(v) << " (" << identifierRegistry->getName(recordValue->getTag()) << ")}";
    return os.str();
}
