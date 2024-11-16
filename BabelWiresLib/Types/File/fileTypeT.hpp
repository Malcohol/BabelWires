/**
 * A type template which provides a convenient way to create a FileType for specific type of contents.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/File/fileType.hpp>
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>

#include <BabelWiresLib/Instance/instance.hpp>

namespace babelwires {

    /// This template provides a convenient way to use FileType without defining and registering a subclass.
    /// T is the class of the content types and this template assumes that instance support has been defined
    /// for T (see instance.hpp). 
    template <typename T> class FileTypeT : public FileType {
      public:
        FileTypeT()
            : FileType(T::getThisType()) {}

        static babelwires::TypeRef getThisType() {
            return babelwires::FileTypeConstructor::makeTypeRef(T::getThisType());
        }

        TypeRef getTypeRef() const override { return getThisType(); }

        /// Implementation Note: Cannot use FileType<T> here because the FileTypeConstructor does not construct
        /// instances of this template but instances of the parent FileType class.
        DECLARE_INSTANCE_BEGIN(FileType)
        DECLARE_INSTANCE_FIELD(Conts, T)
        DECLARE_INSTANCE_END()
    };

} // namespace babelwires
