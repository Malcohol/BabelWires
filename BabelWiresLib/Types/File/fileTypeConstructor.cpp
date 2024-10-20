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

std::unique_ptr<babelwires::Type>
babelwires::FileTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                  const std::vector<const Type*>& typeArguments,
                                                  const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != 1) {
        throw TypeSystemException() << "FileTypeConstructor expects a single type argument but got "
                                    << typeArguments.size();
    }

    if (valueArguments.size() != 0) {
        throw TypeSystemException() << "IntTypeConstructor does not expect any value arguments but got "
                                    << valueArguments.size();
    }

    return std::make_unique<ConstructedType<FileType>>(std::move(newTypeRef), typeArguments[0]->getTypeRef());
}

babelwires::TypeRef babelwires::FileTypeConstructor::makeTypeRef(TypeRef typeOfContents) {
    return TypeRef(getThisIdentifier(), std::move(typeOfContents));
}