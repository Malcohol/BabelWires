/**
 * SelectOptionalsModifierData is used to select a set of optionals in a RecordType
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/selectOptionalsModifierData.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::SelectOptionalsModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeValueArray("activateOptionals", m_activatedOptionals, "activate");
    serializer.serializeValueArray("deactivateOptionals", m_deactivatedOptionals, "deactivate");
}

void babelwires::SelectOptionalsModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_targetPath);
    for (auto it =
             deserializer.deserializeValueArray<ShortId>("activateOptionals", Deserializer::IsOptional::Optional, "activate");
         it.isValid(); ++it) {
        const ShortId temp("__TEMP");
        const ShortId deserializedValue = it.deserializeValue(temp);
        if (deserializedValue == "__TEMP") {
            throw ModelException() << "Problem deserializing activated field";
        }
        m_activatedOptionals.emplace_back(deserializedValue);
    }
    for (auto it =
             deserializer.deserializeValueArray<ShortId>("deactivateOptionals", Deserializer::IsOptional::Optional, "deactivate");
         it.isValid(); ++it) {
        const ShortId temp("__TEMP");
        const ShortId deserializedValue = it.deserializeValue(temp);
        if (deserializedValue == "__TEMP") {
            throw ModelException() << "Problem deserializing deactivated field";
        }
        const auto ait = std::find(m_activatedOptionals.begin(), m_activatedOptionals.end(), deserializedValue);
        if (ait != m_activatedOptionals.end()) {
            m_activatedOptionals.erase(ait);
        } else {
            m_deactivatedOptionals.emplace_back(deserializedValue);
        }
    }
}

void babelwires::SelectOptionalsModifierData::apply(ValueTreeNode* target) const {
    if (auto recordType = target->getType().as<RecordType>()) {
        const TypeSystem& typeSystem = target->getTypeSystem();
        ValueHolder newValue = target->getValue();
        recordType->selectOptionals(typeSystem, newValue, m_activatedOptionals, m_deactivatedOptionals);
        target->setValue(newValue);
        return;
    }
    throw ModelException() << "Cannot activate optionals from a value which does not have optionals";
}

void babelwires::SelectOptionalsModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    for (auto& f : m_activatedOptionals) {
        visitor(f);
    }
}

void babelwires::SelectOptionalsModifierData::setOptionalActivation(ShortId optional, bool isActivate) {
    const auto add = [](auto& vec, ShortId val) {
        if (std::find(vec.begin(), vec.end(), val) == vec.end()) {
            vec.emplace_back(val);
        }
    };
    const auto remove = [](auto& vec, ShortId val) {
        const auto it = std::find(vec.begin(), vec.end(), val);
        if (it != vec.end()) {
            vec.erase(it);
        }
    };

    if (isActivate) {
        add(m_activatedOptionals, optional);
        remove(m_deactivatedOptionals, optional);
    } else {
        add(m_deactivatedOptionals, optional);
        remove(m_activatedOptionals, optional);
    }
} 

void babelwires::SelectOptionalsModifierData::resetOptionalActivation(ShortId optional) {
    const auto remove = [](auto& vec, ShortId val) {
        const auto it = std::find(vec.begin(), vec.end(), val);
        if (it != vec.end()) {
            vec.erase(it);
            return true;
        }
        return false;
    };

    if (!remove(m_activatedOptionals, optional)) {
        const bool wasRemoved = remove(m_deactivatedOptionals, optional);
        assert(wasRemoved && "Optional was not in either list");
    }
}

bool babelwires::SelectOptionalsModifierData::isDefaultState() const {
    return getOptionalActivationData().empty();
}

std::map<babelwires::ShortId, bool> babelwires::SelectOptionalsModifierData::getOptionalActivationData() const {
    std::map<ShortId, bool> result;
    for (const auto& opt : m_activatedOptionals) {
        result[opt] = true;
    }
    for (const auto& opt : m_deactivatedOptionals) {
        result[opt] = false;
    }
    return result;
}
