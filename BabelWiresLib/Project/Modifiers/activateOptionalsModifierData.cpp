/**
 * ActivateOptionalsModifierData is used to select a set of optionals in a RecordWithOptionalsFeature
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
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
    if (RecordWithOptionalsFeature* record = targetFeature->as<RecordWithOptionalsFeature>()) {
        std::vector<ShortId> availableOptionals = record->getOptionalFields();
        std::sort(availableOptionals.begin(), availableOptionals.end());
        auto ait = availableOptionals.begin();

        std::vector<ShortId> optionalsToEnsureActivated = m_selectedOptionals;
        std::sort(optionalsToEnsureActivated.begin(), optionalsToEnsureActivated.end());
        auto it = optionalsToEnsureActivated.begin();

        std::vector<ShortId> missingOptionals;

        while ((ait != availableOptionals.end()) && (it != optionalsToEnsureActivated.end())) {
            if (*ait == *it) {
                if (!record->isActivated(*ait)) {
                    record->activateField(*ait);
                }
                ++ait;
                ++it;
            } else if (*ait < *it) {
                if (record->isActivated(*ait)) {
                    record->deactivateField(*ait);
                }
                ++ait;
            } else {
                missingOptionals.emplace_back(*it);
                ++it;
            }
        }
        while (ait != availableOptionals.end()) {
            if (record->isActivated(*ait)) {
                record->deactivateField(*ait);
            }
            ++ait;
        }
        while (it != optionalsToEnsureActivated.end()) {
            missingOptionals.emplace_back(*it);
            ++it;
        }
        if (!missingOptionals.empty()) {
            if (missingOptionals.size() == optionalsToEnsureActivated.size()) {
                throw ModelException() << "None of the optionals could be activated";
            } else {
                // TODO Warn? Will need to pass the logger into apply.
            }
        }
        return;
    } else if (auto valueFeature = targetFeature->as<ValueFeature>()) {
        if (auto recordType = valueFeature->getType().as<RecordType>()) {
            const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*valueFeature);
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
