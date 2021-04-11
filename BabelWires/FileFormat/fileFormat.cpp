/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/FileFormat/fileFormat.hpp"

babelwires::FileFormatRegistry::FileFormatRegistry()
    : FileTypeRegistry("File Format Registry") {}

babelwires::FileFeatureFactoryRegistry::FileFeatureFactoryRegistry()
    : Registry<FileFeatureFactory>("File Feature Factory Registry"){};

babelwires::FileFormat::FileFormat(std::string identifier, std::string name, VersionNumber version,
                                   Extensions extensions)
    : FileTypeEntry(std::move(identifier), std::move(name), version, std::move(extensions)) {}

babelwires::FileFeatureFactory::FileFeatureFactory(std::string identifier, std::string name, VersionNumber version)
    : RegistryEntry(std::move(identifier), std::move(name), version) {}
