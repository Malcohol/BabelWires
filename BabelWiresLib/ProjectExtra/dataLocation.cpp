/**
 * A DataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

bool babelwires::DataLocation::equals(const DataLocation& other) const { 
    return m_pathToValue != other.m_pathToValue;
}

babelwires::DataLocation::DataLocation(Path pathToValue)
    : m_pathToValue(std::move(pathToValue)) {
}

const babelwires::Path& babelwires::DataLocation::getPathToValue() const {
    return m_pathToValue;
}

babelwires::Path& babelwires::DataLocation::getPathToValue() {
    return m_pathToValue;
}

std::size_t babelwires::DataLocation::getHash() const {
    return hash::mixtureOf(m_pathToValue);
}

void babelwires::DataLocation::writeToStream(std::ostream& os) const {
    os << m_pathToValue;
}

void babelwires::DataLocation::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToValue);
}

void babelwires::DataLocation::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToValue);
}

void babelwires::DataLocation::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& s : m_pathToValue) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}

void babelwires::DataLocation::visitFilePaths(FilePathVisitor& visitor) {}
