/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ValueAssignmentData::ValueAssignmentData() = default;

babelwires::ValueAssignmentData::ValueAssignmentData(std::unique_ptr<EditableValue> value)
    : m_value(std::move(value)) {
    assert(m_value->asEditableValue());
}

babelwires::ValueAssignmentData::ValueAssignmentData(EditableValueHolder value)
    : m_value(std::move(value)) {
    assert(m_value->asEditableValue());
}

void babelwires::ValueAssignmentData::apply(ValueTreeNode* target) const {
    target->setValue(m_value);
}

void babelwires::ValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeObject(m_value->is<EditableValue>(), "value");
}

void babelwires::ValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_targetPath);
    m_value = deserializer.deserializeObject<EditableValue>("value");
}

void babelwires::ValueAssignmentData::visitIdentifiers(IdentifierVisitor& visitor) {
    LocalModifierData::visitIdentifiers(visitor);
    m_value.visitIdentifiers(visitor);
}

void babelwires::ValueAssignmentData::visitFilePaths(FilePathVisitor& visitor) {
    LocalModifierData::visitFilePaths(visitor);
    m_value.visitFilePaths(visitor);
}

babelwires::EditableValueHolder babelwires::ValueAssignmentData::getValue() const {
    return m_value;
}