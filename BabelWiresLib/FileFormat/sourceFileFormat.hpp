/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Registry/fileTypeRegistry.hpp>
#include <Common/productInfo.hpp>

#include <istream>
#include <string>
#include <vector>

namespace babelwires {
    class DataSource;
    struct UserLogger;
} // namespace babelwires

namespace babelwires {
    class FileFeature;
    struct ProjectContext;

    /// Format which can create a feature by loading a file.
    class SourceFileFormat : public FileTypeEntry, ProductInfo {
      public:
        SourceFileFormat(LongId identifier, VersionNumber version, Extensions extensions);
        virtual std::unique_ptr<babelwires::FileFeature> loadFromFile(DataSource& dataSource, const ProjectContext& projectContext,
                                                                      UserLogger& userLogger) const = 0;
    };

    /// Registry of SourceFileFormats.
    class SourceFileFormatRegistry : public FileTypeRegistry<SourceFileFormat> {
      public:
        SourceFileFormatRegistry();
    };
}