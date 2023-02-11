/**
 * A ComplexValueEditorData carries enough data to reconstruct a ComplexValueEditor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

babelwires::ComplexValueEditorData::ComplexValueEditorData(ElementId elementId, FeaturePath pathToValue)
    : m_elementId(elementId)
    , m_pathToValue(std::move(pathToValue)) {

}

babelwires::ElementId babelwires::ComplexValueEditorData::getElementId() const {
    return m_elementId;
}

const babelwires::FeaturePath& babelwires::ComplexValueEditorData::getPathToValue() const {
    return m_pathToValue;
}

std::size_t babelwires::ComplexValueEditorData::getHash() const {
    return hash::mixtureOf(m_elementId, m_pathToValue);
}

void babelwires::ComplexValueEditorData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_elementId);
    serializer.serializeValue("path", m_pathToValue);
}

void babelwires::ComplexValueEditorData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_elementId);
    deserializer.deserializeValue("path", m_pathToValue);
}

void babelwires::ComplexValueEditorData::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& s : m_pathToValue) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}

void babelwires::ComplexValueEditorData::visitFilePaths(FilePathVisitor& visitor) {}
