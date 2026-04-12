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

#include <istream>
#include <string>
#include <vector>

#include <filesystem>

namespace babelwires {
    struct UserLogger;
} // namespace babelwires

namespace babelwires {
    class ValueTreeRoot;
    struct Context;

    class BABELWIRESLIB_API SourceFileFormat : public FileTypeEntry, ProductInfo {
      public:
        SourceFileFormat(LongId identifier, VersionNumber version, Extensions extensions);
        virtual ResultT<std::unique_ptr<babelwires::ValueTreeRoot>> loadFromFile(const std::filesystem::path& path, const Context& context,
                                                                      UserLogger& userLogger) const = 0;
    };

    class BABELWIRESLIB_API SourceFileFormatRegistry : public FileTypeRegistry<SourceFileFormat> {
      public:
        SourceFileFormatRegistry();
    };

}