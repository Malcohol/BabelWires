/**
 * A FileFeature carries a file format identifier, so its contents can be loaded and saved.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/FileFormat/fileFeature.hpp"

babelwires::FileFeature::FileFeature(std::string fileFormatIdentifier)
    : m_factoryIdentifier(fileFormatIdentifier) {}

std::string babelwires::FileFeature::getFileFormatIdentifier() const {
    return m_factoryIdentifier;
}
