/**
 * A RecordFeature carries a sequence of features identified by FeatureIdentifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Features/recordFeature.hpp"

#include "BabelWires/Features/modelExceptions.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"

#include "Common/Utilities/hash.hpp"
#include "Common/types.hpp"

int babelwires::RecordFeature::getNumFeatures() const {
    return m_fields.size();
}

babelwires::Feature* babelwires::RecordFeature::doGetFeature(int i) {
    return m_fields.at(i).m_feature.get();
}

const babelwires::Feature* babelwires::RecordFeature::doGetFeature(int i) const {
    return m_fields.at(i).m_feature.get();
}

babelwires::FieldIdentifier babelwires::RecordFeature::getFieldIdentifier(int i) const {
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

namespace {

    template <typename R>
    typename babelwires::CopyConst<R, babelwires::Feature>::type*
    tryGetChildFromStepT(R* record, const babelwires::PathStep& step) {
        if (const babelwires::FieldIdentifier* field = step.asField()) {
            const int childIndex = record->getChildIndexFromStep(*field);
            if (childIndex >= 0) {
                return record->getFeature(childIndex);
            }
        }
        return nullptr;
    }

} // namespace

babelwires::Feature* babelwires::RecordFeature::tryGetChildFromStep(const PathStep& step) {
    return tryGetChildFromStepT(this, step);
}

const babelwires::Feature* babelwires::RecordFeature::tryGetChildFromStep(const PathStep& step) const {
    return tryGetChildFromStepT(this, step);
}

int babelwires::RecordFeature::getChildIndexFromStep(const FieldIdentifier& identifier) const {
    for (int i = 0; i < getNumFeatures(); ++i) {
        FieldIdentifier f = getFieldIdentifier(i);
        if (f == identifier) {
            // Since step has resolved, ensure it gets the correct discriminator.
            f.copyDiscriminatorTo(identifier);
            return i;
        }
    }
    return -1;
}

void babelwires::RecordFeature::addFieldInternal(std::unique_ptr<Feature> f, const FieldIdentifier& identifier) {
    assert(identifier.getDiscriminator() != 0);
    f->setOwner(this);
    m_fields.emplace_back(Field{identifier, std::move(f)});
    setChanged(Changes::StructureChanged);
}

std::size_t babelwires::RecordFeature::doGetHash() const {
    std::size_t hash = hash::mixtureOf(static_cast<const char*>("Record"));
    // We assume that records do no change their vector of fields.
    for (const auto& f : m_fields) {
        hash::mixInto(hash, f.m_feature->getHash());
    }
    return hash;
}
