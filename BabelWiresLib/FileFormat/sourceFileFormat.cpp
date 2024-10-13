/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>

babelwires::SourceFileFormatRegistry::SourceFileFormatRegistry()
    : FileTypeRegistry("File Format Registry") {}

babelwires::SourceFileFormat::SourceFileFormat(LongId identifier, VersionNumber version, Extensions extensions)
    : FileTypeEntry(identifier, version, std::move(extensions)) {}


babelwires::SourceFileFormatRegistry2::SourceFileFormatRegistry2()
    : FileTypeRegistry("File Format Registry") {}

babelwires::SourceFileFormat2::SourceFileFormat2(LongId identifier, VersionNumber version, Extensions extensions)
    : FileTypeEntry(identifier, version, std::move(extensions)) {}
