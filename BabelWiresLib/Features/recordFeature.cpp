/**
 * A RecordFeature carries a sequence of features identified by FeatureIdentifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/recordFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Hash/hash.hpp>
#include <Common/types.hpp>

int babelwires::RecordFeature::getNumFeatures() const {
    return m_fields.size();
}

void babelwires::RecordFeature::doSetToDefault() {
    setSubfeaturesToDefault();
}

void babelwires::RecordFeature::doSetToDefaultNonRecursive() {}

babelwires::Feature* babelwires::RecordFeature::doGetFeature(int i) {
    return m_fields.at(i).m_feature.get();
}

const babelwires::Feature* babelwires::RecordFeature::doGetFeature(int i) const {
    return m_fields.at(i).m_feature.get();
}

babelwires::ShortId babelwires::RecordFeature::getFieldIdentifier(int i) const {
    return m_fields.at(i).m_identifier;
}

babelwires::PathStep babelwires::RecordFeature::getStepToChild(const Feature* child) const {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeature(i) == child) {
            return PathStep(getFieldIdentifier(i));
        }
    }
    throw ModelException() << "Child not found in owner";
}

int babelwires::RecordFeature::getChildIndexFromStep(const PathStep& step) const {
    if (const babelwires::ShortId* identifier = step.asField()) {
        for (int i = 0; i < getNumFeatures(); ++i) {
            ShortId f = getFieldIdentifier(i);
            if (f == *identifier) {
                // Since step has resolved, ensure it gets the correct discriminator.
                f.copyDiscriminatorTo(*identifier);
                return i;
            }
        }
    }
    return -1;
}

void babelwires::RecordFeature::addFieldInternal(Field f, int index) {
    assert((f.m_identifier.getDiscriminator() != 0) && "Only registered identfiers can be used as field identifiers");
    assert((tryGetChildFromStep(PathStep(f.m_identifier)) == nullptr) &&
           "There's an alredy a field with that field identifier");
    f.m_feature->setOwner(this);
    if (index == -1) {
        m_fields.emplace_back(std::move(f));
    } else {
        m_fields.emplace(m_fields.begin() + index, std::move(f));
    }
    setChanged(Changes::StructureChanged);
}

void babelwires::RecordFeature::addFieldAndIndexInternal(FieldAndIndex fieldAndIndex) {
    const int targetIndex = fieldAndIndex.m_index;
    addFieldInternal(std::move(fieldAndIndex), targetIndex);
}

babelwires::RecordFeature::FieldAndIndex babelwires::RecordFeature::removeField(ShortId identifier) {
    int i;
    for (i = 0; i < m_fields.size(); ++i) {
        if (identifier == m_fields[i].m_identifier) {
            break;
        }
    }
    assert((i < m_fields.size()) && "The field to remove was not present");
    FieldAndIndex f{std::move(m_fields[i]), i};
    // Removed fields may still need to be rooted in the hierarchy, so we don't set their owner to null here.
    // This requires the caller to be respectful of how the removed field is treated, since it could
    // propagate changes up the owner chain.
    // TODO: Consider setting to default and then settings owner to null. That might account for the needs of all use-cases.
    m_fields.erase(m_fields.begin() + i);
    setChanged(Changes::StructureChanged);
    return f;
}

std::size_t babelwires::RecordFeature::doGetHash() const {
    std::size_t hash = hash::mixtureOf(static_cast<const char*>("Record"));
    for (const auto& f : m_fields) {
        // Records can change their set of active fields (see RecordWithOptionalsFeature and UnionFeature), so we mix in
        // the field's identifier as well as its hash.
        hash::mixInto(hash, f.m_identifier);
        hash::mixInto(hash, f.m_feature->getHash());
    }
    return hash;
}
