/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Registry/fileTypeRegistry.hpp"
#include "Common/productInfo.hpp"

#include <istream>
#include <string>
#include <vector>

namespace babelwires {
    class DataSource;
    struct UserLogger;
} // namespace babelwires

namespace babelwires {
    class FileFeature;

    /// Format which can create a feature by loading a file.
    class SourceFileFormat : public FileTypeEntry, ProductInfo {
      public:
        SourceFileFormat(std::string identifier, std::string name, VersionNumber version, Extensions extensions);
        virtual std::unique_ptr<babelwires::FileFeature> loadFromFile(DataSource& dataSource,
                                                                      UserLogger& userLogger) const = 0;
    };

    /// Registry of SourceFileFormats.
    class SourceFileFormatRegistry : public FileTypeRegistry<SourceFileFormat> {
      public:
        SourceFileFormatRegistry();
    };

    /// Factories which can create FileFeatures in a default state, and write those features as files.
    class TargetFileFactory : public FileTypeEntry, ProductInfo {
      public:
        TargetFileFactory(std::string identifier, std::string name, VersionNumber version, Extensions extensions);
        virtual std::unique_ptr<FileFeature> createNewFeature() const = 0;
        virtual void writeToFile(const FileFeature& fileFeature, std::ostream& os, UserLogger& userLogger) const = 0;
    };

    /// Registry of TargetFileFactories.
    /// Note: This is not a FileTypeRegistry, since these are not expected to be queried by extension and
    /// more than one can target the same extension.
    class TargetFileFactoryRegistry : public Registry<TargetFileFactory> {
      public:
        TargetFileFactoryRegistry();
    };

} // namespace babelwires
