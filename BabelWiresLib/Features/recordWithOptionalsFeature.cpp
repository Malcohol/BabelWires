/**
 * A RecordWithOptionalsFeature is a Record feature with subfeatures which can be inactive.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <algorithm>

void babelwires::RecordWithOptionalsFeature::addOptionalFieldInternal(FieldAndIndex f) {
    m_optionalFields.emplace_back(f.m_identifier);
    m_inactiveFields.emplace_back(std::move(f));
}

void babelwires::RecordWithOptionalsFeature::activateField(Identifier identifier) {
    const auto it = std::find_if(m_inactiveFields.begin(), m_inactiveFields.end(),
                                 [identifier](const FieldAndIndex& f) { return f.m_identifier == identifier; });
    if(it == m_inactiveFields.end()) {
        throw ModelException() << "The field " << identifier << " is not an inactive optional field, so it cannot be activated";
    }
    std::for_each(it + 1, m_inactiveFields.end(), [](FieldAndIndex& f) { ++f.m_index; });
    addFieldAndIndexInternal(std::move(*it));
    m_inactiveFields.erase(it);
}

void babelwires::RecordWithOptionalsFeature::deactivateField(Identifier identifier) {
    if (!isOptional(identifier) || !isActivated(identifier)) {
        throw ModelException() << "The field " << identifier << " is not an active optional field, so it cannot be deactivated";
    }
    FieldAndIndex f = removeField(identifier);
    // This should maybe be the responsibility of the caller, but doing it here means that the owner is already null,
    // so value changes are not propagated to the record.
    f.m_feature->setToDefault();
    const auto insertionPoint =
        std::upper_bound(m_inactiveFields.begin(), m_inactiveFields.end(), f,
                         [](const FieldAndIndex& a, const FieldAndIndex& b) { return a.m_index < b.m_index; });
    std::for_each(insertionPoint, m_inactiveFields.end(), [](FieldAndIndex& x) { --x.m_index; });
    m_inactiveFields.insert(insertionPoint, std::move(f));
}

bool babelwires::RecordWithOptionalsFeature::isOptional(Identifier identifier) const {
    return std::find(m_optionalFields.begin(), m_optionalFields.end(), identifier) != m_optionalFields.end();
}

bool babelwires::RecordWithOptionalsFeature::isActivated(Identifier identifier) const {
    assert(isOptional(identifier) && "The identifier does not identify an optional field");
    return tryGetChildFromStep(PathStep(identifier));
}

const std::vector<babelwires::Identifier>& babelwires::RecordWithOptionalsFeature::getOptionalFields() const {
    return m_optionalFields;
}

int babelwires::RecordWithOptionalsFeature::getNumInactiveFields() const {
    return m_inactiveFields.size();
}

void babelwires::RecordWithOptionalsFeature::doSetToDefault() {
    setToDefaultNonRecursive();
    setSubfeaturesToDefault();
}

void babelwires::RecordWithOptionalsFeature::doSetToDefaultNonRecursive() {
    for (auto& inactiveField : m_inactiveFields) {
        // After construction, these may not have been set to their default state.
        inactiveField.m_feature->setToDefault();
    }
    for (const auto& f : m_optionalFields) {
        if (isActivated(f)) {
            deactivateField(f);
        }
    }
}
