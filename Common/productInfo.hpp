/**
 * An interface which provides information about a product (e.g. a piece of hardware).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>

namespace babelwires {

    /// An interface which provides information about a product (e.g. a piece of hardware).
    /// Several registries have entries which supply this information.
    class ProductInfo {
      public:
        virtual ~ProductInfo() = default;
        virtual std::string getManufacturerName() const = 0;
        virtual std::string getProductName() const = 0;
    };

} // namespace babelwires
