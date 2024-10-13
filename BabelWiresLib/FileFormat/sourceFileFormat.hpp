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
    class SimpleValueFeature;
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

    class SourceFileFormat2 : public FileTypeEntry, ProductInfo {
      public:
        SourceFileFormat2(LongId identifier, VersionNumber version, Extensions extensions);
        virtual std::unique_ptr<babelwires::SimpleValueFeature> loadFromFile(DataSource& dataSource, const ProjectContext& projectContext,
                                                                      UserLogger& userLogger) const = 0;
    };

    class SourceFileFormatRegistry2 : public FileTypeRegistry<SourceFileFormat2> {
      public:
        SourceFileFormatRegistry2();
    };

}