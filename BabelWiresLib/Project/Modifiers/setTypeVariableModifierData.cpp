/**
 * SetTypeVariableModifierData is used to assign specific types to type variables.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>

#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::SetTypeVariableModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeArray("typeAssignments", m_typeAssignments);
}

void babelwires::SetTypeVariableModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_targetPath);
    auto it = deserializer.deserializeArray<TypeExp>("typeAssignments");
    while (it.isValid()) {
        m_typeAssignments.emplace_back(std::move(*it.getObject()));
        ++it;
    }
}

void babelwires::SetTypeVariableModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    for (auto& typeAssignment : m_typeAssignments) {
        typeAssignment.visitIdentifiers(visitor);
    }
}

void babelwires::SetTypeVariableModifierData::apply(ValueTreeNode* target) const {
    if (auto genericType = target->getType()->tryAs<GenericType>()) {
        ValueHolder newValue = target->getValue();
        if (m_typeAssignments.size() != genericType->getNumVariables()) {
            throw ModelException()
                << "SetTypeVariable modifier has incorrect number of type assignments for Generic Type";
        }
        genericType->setTypeVariableAssignmentAndInstantiate(target->getTypeSystem(), newValue, m_typeAssignments);
        target->setValue(newValue);
    } else {
        throw ModelException()
            << "SetTypeVariable modifier applied to ValueTreeNode which does not have a Generic Type";
    }
}
