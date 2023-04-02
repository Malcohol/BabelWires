/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ValueAssignmentData::ValueAssignmentData() = default;

babelwires::ValueAssignmentData::ValueAssignmentData(std::unique_ptr<Value> value)
    : m_value(std::move(value)) {}

void babelwires::ValueAssignmentData::apply(Feature* targetFeature) const {
    if (SimpleValueFeature* valueFeature = targetFeature->as<SimpleValueFeature>()) {
        valueFeature->setValueHolder(m_value);
    } else {
        throw babelwires::ModelException() << "Could not assign an value to a non-SimpleValueFeature";
    }
}

void babelwires::ValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeObject(*m_value, "value");
}

void babelwires::ValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    m_value = deserializer.deserializeObject<Value>("value");
}

void babelwires::ValueAssignmentData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_value.visitIdentifiers(visitor);
}

void babelwires::ValueAssignmentData::visitFilePaths(FilePathVisitor& visitor) {
    m_value.visitFilePaths(visitor);
}
