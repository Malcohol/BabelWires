/**
 * A type which corresponds to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

namespace babelwires {

    /// A type which wraps the contents of a file.
    /// The FileType has no real functionality of its own and mainly just reserves a row in the feature hierarchy.
    /// The actual file information is stored in the NodeData of the containing FileFeature.
    class FileType : public RecordType {
      public:
        FileType(TypeExp&& typeExp, const TypeSystem& typeSystem, TypeExp typeOfContents);

        /// Returns the empty string, preventing the file from being connected to or from.
        std::string getFlavour() const override;

        /// The identifier of the field which holds the file's contents.
        static babelwires::ShortId getStepToContents();
    };
}
