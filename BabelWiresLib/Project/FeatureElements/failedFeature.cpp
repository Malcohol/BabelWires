/**
 * The FailedFeature is a dummy feature used when the contents of FeatureElement cannot be properly initialized.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/failedFeature.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::FailedFeature::FailedFeature(const ProjectContext& projectContext)
    : RootFeature(projectContext) {
    addField(std::make_unique<RootFeature>(projectContext),
             BW_SHORT_ID("Failed", "Failed", "2d9667c0-0829-48ec-a952-5ba96cb5693f"));
}
