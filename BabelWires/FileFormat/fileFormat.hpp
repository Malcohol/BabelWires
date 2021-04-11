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

    /// Format which knows how to load and save files to disk.
    class FileFormat : public FileTypeEntry, ProductInfo {
      public:
        FileFormat(std::string identifier, std::string name, VersionNumber version, Extensions extensions);
        virtual std::unique_ptr<babelwires::FileFeature> loadFromFile(DataSource& dataSource,
                                                                      UserLogger& userLogger) const = 0;
        virtual void writeToFile(const FileFeature& fileFeature, std::ostream& os, UserLogger& userLogger) const = 0;
    };

    /// Registry of file formats.
    class FileFormatRegistry : public FileTypeRegistry<FileFormat> {
      public:
        FileFormatRegistry();
    };

    /// Factories which can create FileFeatures in a default state.
    class FileFeatureFactory : public RegistryEntry, ProductInfo {
      public:
        FileFeatureFactory(std::string identifier, std::string name, VersionNumber version);
        virtual std::unique_ptr<FileFeature> createNewFeature() const = 0;
    };

    /// Registry of FileFeatureFactories.
    class FileFeatureFactoryRegistry : public Registry<FileFeatureFactory> {
      public:
        FileFeatureFactoryRegistry();
    };

} // namespace babelwires
