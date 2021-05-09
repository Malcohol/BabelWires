#include "BabelWires/Features/recordWithOptionalsFeature.hpp"

void babelwires::RecordWithOptionalsFeature::addOptionalFieldInternal(FieldAndIndex f) {
    m_optionalFields.emplace_back(f.m_identifier);
    m_inactiveFields.emplace_back(std::move(f));
}

void babelwires::RecordWithOptionalsFeature::activateField(FieldIdentifier identifier) {
    assert((std::find(m_optionalFields.begin(), m_optionalFields.end(), identifier) != m_optionalFields.end()) &&
           "You cannot activate a field which is not optional");
    const auto it = std::find_if(m_inactiveFields.begin(), m_inactiveFields.end(),
                                 [identifier](const FieldAndIndex& f) { return f.m_identifier == identifier; });
    assert((it != m_inactiveFields.end()) && "The optional to activate was not found");
    std::for_each(it + 1, m_inactiveFields.end(), [](FieldAndIndex& f) { ++f.m_index; });
    addFieldInternal(std::move(*it), it->m_index);
    m_inactiveFields.erase(it);
}

void babelwires::RecordWithOptionalsFeature::deactivateField(FieldIdentifier identifier) {
    assert((std::find(m_optionalFields.begin(), m_optionalFields.end(), identifier) != m_optionalFields.end()) &&
           "You cannot activate a field which is not optional");
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

bool babelwires::RecordWithOptionalsFeature::isOptional(FieldIdentifier identifier) const {
    return std::find(m_optionalFields.begin(), m_optionalFields.end(), identifier) != m_optionalFields.end();
}

bool babelwires::RecordWithOptionalsFeature::isActivated(FieldIdentifier identifier) const {
    assert(isOptional(identifier) && "The identifier does not identify an optional field");
    return tryGetChildFromStep(PathStep(identifier));
}

const std::vector<babelwires::FieldIdentifier>& babelwires::RecordWithOptionalsFeature::getOptionalFields() const {
    return m_optionalFields;
}
