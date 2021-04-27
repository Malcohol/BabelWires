/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/FileFormat/fileFormat.hpp"

babelwires::SourceFileFormatRegistry::SourceFileFormatRegistry()
    : FileTypeRegistry("File Format Registry") {}

babelwires::TargetFileFactoryRegistry::TargetFileFactoryRegistry()
    : Registry("File Feature Factory Registry"){};

babelwires::SourceFileFormat::SourceFileFormat(std::string identifier, std::string name, VersionNumber version,
                                   Extensions extensions)
    : FileTypeEntry(std::move(identifier), std::move(name), version, std::move(extensions)) {}

babelwires::TargetFileFactory::TargetFileFactory(std::string identifier, std::string name, VersionNumber version, Extensions extensions)
    : FileTypeEntry(std::move(identifier), std::move(name), version, std::move(extensions)) {}
