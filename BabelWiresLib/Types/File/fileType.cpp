/**
 * A type which corresponds to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/File/fileType.hpp>

babelwires::FileType::FileType(TypeExp&& typeExpOfThis, const TypeSystem& typeSystem, TypeExp typeOfContents)
    : RecordType(std::move(typeExpOfThis), typeSystem, {{getStepToContents(), typeOfContents}}) {}

std::string babelwires::FileType::getFlavour() const {
    return "";
}

babelwires::ShortId babelwires::FileType::getStepToContents() {
    return BW_SHORT_ID("Conts", "Contents", "5144fef7-7361-495b-8967-ac6a28aa13bd");
}
