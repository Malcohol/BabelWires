/**
 * A TypeConstructor which constructs a file type to wrap a particular value type for its contents.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>

babelwires::TypeConstructor::TypeConstructorResult
babelwires::FileTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                               const TypeConstructorArguments& arguments,
                                               const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 1) {
        throw TypeSystemException() << "FileTypeConstructor expects a single type argument but got "
                                    << arguments.getTypeArguments().size();
    }

    if (arguments.getValueArguments().size() != 0) {
        throw TypeSystemException() << "IntTypeConstructor does not expect any value arguments but got "
                                    << arguments.getValueArguments().size();
    }

    return std::make_unique<ConstructedType<FileType>>(std::move(newTypeRef), arguments.getTypeArguments()[0]);
}

babelwires::TypeRef babelwires::FileTypeConstructor::makeTypeRef(TypeRef typeOfContents) {
    return TypeRef(getThisIdentifier(), std::move(typeOfContents));
}