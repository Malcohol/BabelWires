/**
 * SetTypeVariableModifierData is used to select the variant of a RecordWithVariantsType.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::SetTypeVariableModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeObject(m_typeRef);
    serializer.serializeValue("variableIndex", m_variableIndex);
}

void babelwires::SetTypeVariableModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_targetPath);
    m_typeRef = std::move(*deserializer.deserializeObject<TypeRef>());
    deserializer.deserializeValue("variableIndex", m_variableIndex);
}

void babelwires::SetTypeVariableModifierData::apply(ValueTreeNode* target) const {
    if (auto genericType = target->getType().as<GenericType>()) {
        if (m_variableIndex >= genericType->getNumVariables()) {
            throw ModelException() << "SetTypeVariable modifier applied with invalid variable index: "
                                   << m_variableIndex;
        }
        ValueHolder newValue = target->getValue();
        genericType->instantiate(target->getTypeSystem(), newValue, m_variableIndex, m_typeRef);
        target->setValue(newValue);
    } else {
        throw ModelException() << "SetTypeVariable modifier applied to ValueTreeNode which does not have a Generic Type";
    }
}

void babelwires::SetTypeVariableModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    m_typeRef.visitIdentifiers(visitor);
}
