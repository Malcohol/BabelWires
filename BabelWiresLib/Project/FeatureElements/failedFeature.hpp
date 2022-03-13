/**
 * The FailedFeature is a dummy feature used when the contents of FeatureElement cannot be properly initialized.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/rootFeature.hpp"

namespace babelwires {

    /// A dummy feature to use when the contents of FeatureElement cannot be properly initialized.
    struct FailedFeature : RootFeature {
        FailedFeature(const ProjectContext& projectContext);
    };

} // namespace babelwires
