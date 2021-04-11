/**
 * The FailedFeature is a dummy feature used when the contents of FeatureElement cannot be properly initialized.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/failedFeature.hpp"

#include "BabelWires/Features/Path/fieldName.hpp"

babelwires::FailedFeature::FailedFeature() {
    addField(std::make_unique<RecordFeature>(), FIELD_NAME("Failed", "Failed", "2d9667c0-0829-48ec-a952-5ba96cb5693f"));
}
