/**
 * A FileFeature carries a file format identifier, so its contents can be loaded and saved.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/recordFeature.hpp"

namespace babelwires {

    class FileFormat;

    /// A record feature which has a corresponding file format, and therefore can be loaded and saved.
    class FileFeature : public RecordFeature {
      public:
        FileFeature(std::string fileFormatIdentifier);

        /// Return the identifier of the file format which knows how to load and save this type of feature.
        std::string getFileFormatIdentifier() const;

      private:
        /// The file format which knows how to load and save this type of feature.
        std::string m_factoryIdentifier;
    };

} // namespace babelwires
