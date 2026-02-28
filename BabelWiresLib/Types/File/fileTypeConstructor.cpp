/**
 * A TypeConstructor which constructs a file type to wrap a particular value type for its contents.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>

#include <BabelWiresLib/Types/File/fileType.hpp>

#include <BaseLib/Result/error.hpp>

babelwires::ResultT<babelwires::TypePtr>
babelwires::FileTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                               const TypeConstructorArguments& arguments,
                                               const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 1) {
        return Error() << "FileTypeConstructor expects a single type argument but got "
                       << arguments.getTypeArguments().size();
    }

    if (arguments.getValueArguments().size() != 0) {
        return Error() << "IntTypeConstructor does not expect any value arguments but got "
                       << arguments.getValueArguments().size();
    }

    return makeType<FileType>(std::move(newTypeExp), typeSystem, arguments.getTypeArguments()[0]);
}

babelwires::TypeExp babelwires::FileTypeConstructor::makeTypeExp(TypeExp typeOfContents) {
    return TypeExp(getThisIdentifier(), std::move(typeOfContents));
}