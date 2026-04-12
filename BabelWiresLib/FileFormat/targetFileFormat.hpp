/**
 * Factories for code which knows how to create, load and save FileFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BaseLib/Registry/fileTypeRegistry.hpp>
#include <BaseLib/Result/result.hpp>
#include <BaseLib/productInfo.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace babelwires {
    class DataSource;
    struct UserLogger;
} // namespace babelwires

namespace babelwires {
    class ValueTreeRoot;
    class Context;

    /// Factories which can create FileFeatures in a default state, and write those features as files.
    class BABELWIRESLIB_API TargetFileFormat : public FileTypeEntry, ProductInfo {
      public:
        TargetFileFormat(LongId identifier, VersionNumber version, Extensions extensions);
        virtual std::unique_ptr<ValueTreeRoot> createNewValue(const Context& context) const = 0;
        virtual Result writeToFile(const Context& context, UserLogger& userLogger,
                                   const ValueTreeRoot& contents, const std::filesystem::path& path) const = 0;
    };

    /// Registry of TargetFileFactories.
    /// Note: This is not a FileTypeRegistry, since these are not expected to be queried by extension and
    /// more than one can target the same extension.
    class BABELWIRESLIB_API TargetFileFormatRegistry : public Registry<TargetFileFormat> {
      public:
        TargetFileFormatRegistry();
    };

} // namespace babelwires
