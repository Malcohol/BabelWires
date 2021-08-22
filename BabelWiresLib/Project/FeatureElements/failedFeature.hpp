/**
 * The FailedFeature is a dummy feature used when the contents of FeatureElement cannot be properly initialized.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/recordFeature.hpp"

namespace babelwires {

    /// A dummy feature to use when the contents of FeatureElement cannot be properly initialized.
    struct FailedFeature : RecordFeature {
        FailedFeature();
    };

} // namespace babelwires
