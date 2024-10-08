/**
 * ActivateOptionalsModifierData is used to select a set of optionals in a RecordType
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::ActivateOptionalsModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValueArray("optionals", m_selectedOptionals, "activate");
}

void babelwires::ActivateOptionalsModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    for (auto it =
             deserializer.deserializeValueArray<ShortId>("optionals", Deserializer::IsOptional::Optional, "activate");
         it.isValid(); ++it) {
        ShortId temp("__TEMP");
        m_selectedOptionals.emplace_back(it.deserializeValue(temp));
        assert((m_selectedOptionals.back() != "__TEMP") && "Problem deserializing optional fields");
    }
}

void babelwires::ActivateOptionalsModifierData::apply(Feature* targetFeature) const {
    if (auto valueFeature = targetFeature->as<ValueFeature>()) {
        if (auto recordType = valueFeature->getType().as<RecordType>()) {
            const TypeSystem& typeSystem = valueFeature->getTypeSystem();
            ValueHolder newValue = valueFeature->getValue();
            recordType->ensureActivated(typeSystem, newValue, m_selectedOptionals);
            valueFeature->setValue(newValue);
            return;
        }
    }
    throw ModelException() << "Cannot activate optionals from a feature which does not have optionals";
}

void babelwires::ActivateOptionalsModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    for (auto& f : m_selectedOptionals) {
        visitor(f);
    }
}
