/**
 * A RootFeature is a feature intended to exist at the root of a feature hierarchy.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/rootFeature.hpp>

babelwires::RootFeature::RootFeature(const ProjectContext& projectContext)
    : m_projectContext(projectContext) {}

const babelwires::RootFeature* babelwires::RootFeature::tryGetRootFeatureAt(const Feature& feature) {
    const Feature* current;
    const Feature* owner = &feature;
    do {
        current = owner;
        owner = current->getOwner();
    } while (owner);
    return current->as<RootFeature>();
}

/// Get the project context carried by this object.
const babelwires::ProjectContext& babelwires::RootFeature::getProjectContextAt(const Feature& feature) {
    const RootFeature* rootFeature = tryGetRootFeatureAt(feature);
    assert(rootFeature && "You cannot only call getProjectContextAt in a feature hierarchy with a RootFeature at its root");
    return rootFeature->m_projectContext;
}

babelwires::Feature::Style babelwires::RootFeature::getStyle() const {
    return babelwires::Feature::Style::isInlined;
}
