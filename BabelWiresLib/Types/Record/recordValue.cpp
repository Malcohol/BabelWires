/**
 * RecordValues are compound values containing a sequence of named children.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordValue.hpp>

babelwires::ValueHolder& babelwires::RecordValue::getValue(ShortId fieldId) {
    auto it = m_fieldValues.find(fieldId);
    assert((it != m_fieldValues.end()) && "Field not found in RecordValue");
    return it->second;
}

const babelwires::ValueHolder& babelwires::RecordValue::getValue(ShortId fieldId) const {
    auto it = m_fieldValues.find(fieldId);
    assert((it != m_fieldValues.end()) && "Field not found in RecordValue");
    return it->second;
}

babelwires::ValueHolder* babelwires::RecordValue::tryGetValue(ShortId fieldId) {
    auto it = m_fieldValues.find(fieldId);
    if (it != m_fieldValues.end()) {
        return &it->second;
    }
    return nullptr;
}

const babelwires::ValueHolder* babelwires::RecordValue::tryGetValue(ShortId fieldId) const {
    auto it = m_fieldValues.find(fieldId);
    if (it != m_fieldValues.end()) {
        return &it->second;
    }
    return nullptr;
}

void babelwires::RecordValue::setValue(ShortId fieldId, ValueHolder newValue) {
    m_fieldValues.insert({fieldId, newValue});
}

void babelwires::RecordValue::removeValue(ShortId fieldId) {
    auto it = m_fieldValues.find(fieldId);
    assert((it != m_fieldValues.end()) && "Fields not found in RecordValue");
    m_fieldValues.erase(it);
}

std::size_t babelwires::RecordValue::getHash() const {
    /// Unordered_maps are not deterministically sorted, so we need to sort before traversing for a hash calculation.
    using FieldRef = std::tuple<ShortId, const ValueHolder*>;
    std::vector<FieldRef> sortedFields;
    sortedFields.reserve(m_fieldValues.size());
    for (const auto& f : m_fieldValues) {
        sortedFields.emplace_back(FieldRef{f.first, &f.second});
    }
    std::sort(sortedFields.begin(), sortedFields.end(),
              [](const FieldRef& a, const FieldRef& b) { return std::get<0>(a) < std::get<0>(b); });

    std::size_t hash = hash::mixtureOf(sortedFields.size());
    for (auto f : sortedFields) {
        hash::mixInto(hash, std::get<0>(f), *std::get<1>(f));
    }
    return hash;
}

bool babelwires::RecordValue::operator==(const Value& other) const {
    const RecordValue *const otherRecord = other.as<RecordValue>();
    if (otherRecord == nullptr) {
        return false;
    }
    if (m_fieldValues.size() != otherRecord->m_fieldValues.size()) {
        return false;
    }
    for (const auto& f : m_fieldValues) {
        const ValueHolder *const otherField = otherRecord->tryGetValue(f.first);
        if (!otherField) {
            return false;
        }
        if (f.second != *otherField) {
            return false;
        }
    }
    return true;
}
