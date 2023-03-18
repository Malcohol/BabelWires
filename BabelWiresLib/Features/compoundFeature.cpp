/**
 * A CompoundFeature is a feature which contains other features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/compoundFeature.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

namespace {
    void checkIndex(const babelwires::CompoundFeature* f, int i) {
        if ((i < 0) || (i >= f->getNumFeatures())) {
            throw babelwires::ModelException()
                << "Compound feature with " << f->getNumFeatures() << " children queried by index " << i;
        }
    }

} // namespace

babelwires::Feature* babelwires::CompoundFeature::getFeature(int i) {
    checkIndex(this, i);
    return doGetFeature(i);
}

const babelwires::Feature* babelwires::CompoundFeature::getFeature(int i) const {
    checkIndex(this, i);
    return doGetFeature(i);
}

void babelwires::CompoundFeature::setSubfeaturesToDefault() {
    for (auto&& child : subfeatures(*this)) {
        child->setToDefault();
    }
}

void babelwires::CompoundFeature::doClearChanges() {
    Feature::doClearChanges();
    for (auto&& child : subfeatures(*this)) {
        child->clearChanges();
    }
}

babelwires::Feature& babelwires::CompoundFeature::getChildFromStep(const PathStep& step) {
    if (Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::Feature& babelwires::CompoundFeature::getChildFromStep(const PathStep& step) const {
    if (const Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}
