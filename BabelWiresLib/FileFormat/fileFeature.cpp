/**
 * A FileFeature carries a file format identifier, so its contents can be loaded and saved.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/FileFormat/fileFeature.hpp>

babelwires::FileFeature::FileFeature(const ProjectContext& projectContext, LongId fileFormatIdentifier)
    : RootFeature(projectContext)
    , m_factoryIdentifier(fileFormatIdentifier) {}

babelwires::LongId babelwires::FileFeature::getFileFormatIdentifier() const {
    return m_factoryIdentifier;
}
