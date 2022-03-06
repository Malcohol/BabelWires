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

/// Get the project context carried by this object.
const babelwires::ProjectContext& babelwires::RootFeature::getProjectContextAtRoot() const {
    return m_projectContext;
}
