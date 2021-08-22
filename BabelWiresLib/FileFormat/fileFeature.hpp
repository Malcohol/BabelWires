/**
 * A FileFeature carries a file format identifier, so its contents can be loaded and saved.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/recordFeature.hpp"

namespace babelwires {

    class SourceFileFormat;

    /// A record feature which has a corresponding file format, and therefore can be loaded or saved.
    /// This is treated specially so a row with file operations is available in the UI.
    // TODO: abandon this and do all special casing in the UI.
    class FileFeature : public RecordFeature {
      public:
        FileFeature(std::string fileFormatIdentifier);

        /// Return the identifier of the file format which knows how to load and save this type of feature.
        // TODO No longer used.
        std::string getFileFormatIdentifier() const;

      private:
        /// The file format which knows how to load or save this type of feature.
        // TODO This is no longer used. Everywhere that needs it, can obtain it from the element.
        std::string m_factoryIdentifier;
    };

} // namespace babelwires
