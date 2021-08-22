/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/FileFormat/targetFileFormat.hpp"

babelwires::TargetFileFormatRegistry::TargetFileFormatRegistry()
    : Registry("File Feature Factory Registry"){};

babelwires::TargetFileFormat::TargetFileFormat(std::string identifier, std::string name, VersionNumber version, Extensions extensions)
    : FileTypeEntry(std::move(identifier), std::move(name), version, std::move(extensions)) {}
