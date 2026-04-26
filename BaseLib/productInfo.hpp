/**
 * An interface which provides information about a product (e.g. a piece of hardware).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <string>

namespace babelwires {

    /// An interface which provides information about a product (e.g. a piece of hardware).
    /// Several registries have entries which supply this information.
    // TODO - The semantics is too specific and rigid, especially as this is in BaseLib.
    class BASELIB_API ProductInfo {
      public:
        virtual ~ProductInfo() = default;
        virtual std::string getManufacturerName() const = 0;
        virtual std::string getProductName() const = 0;
    };

} // namespace babelwires
