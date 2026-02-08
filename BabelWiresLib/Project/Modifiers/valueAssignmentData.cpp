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

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

babelwires::ValueAssignmentData::ValueAssignmentData() = default;

babelwires::ValueAssignmentData::ValueAssignmentData(std::unique_ptr<Value> value)
    : m_value(std::move(value)) {
    assert(m_value->tryGetAsEditableValue());
}

babelwires::ValueAssignmentData::ValueAssignmentData(ValueHolder value)
    : m_value(std::move(value)) {
    assert(m_value->tryGetAsEditableValue());
}

void babelwires::ValueAssignmentData::apply(ValueTreeNode* target) const {
    target->setValue(m_value);
}

void babelwires::ValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeObject(m_value->getAsEditableValue(), "value");
}

babelwires::Result babelwires::ValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.deserializeValue("path", m_targetPath));
    // The uniquePtrCast is needed to disambiguate the assignment operator.
    ASSIGN_OR_ERROR(auto valuePtr, deserializer.deserializeObject<EditableValue>("value"));
    m_value = uniquePtrCast<Value>(std::move(valuePtr));
    return {};
}

void babelwires::ValueAssignmentData::visitIdentifiers(IdentifierVisitor& visitor) {
    LocalModifierData::visitIdentifiers(visitor);
    m_value.visitIdentifiers(visitor);
}

void babelwires::ValueAssignmentData::visitFilePaths(FilePathVisitor& visitor) {
    LocalModifierData::visitFilePaths(visitor);
    m_value.visitFilePaths(visitor);
}

babelwires::ValueHolder babelwires::ValueAssignmentData::getValue() const {
    return m_value;
}