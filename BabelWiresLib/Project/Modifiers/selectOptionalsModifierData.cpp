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

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

namespace {
    struct SerializableOptional : babelwires::Serializable {
        SERIALIZABLE_ABSTRACT(SerializableOptional, void);
        DOWNCASTABLE_TYPE_HIERARCHY(SerializableOptional);
        void serializeContents(babelwires::Serializer& serializer) const {
            serializer.serializeValue("optional", m_optional);
        }
        void deserializeContents(babelwires::Deserializer& deserializer) {
            THROW_ON_ERROR(deserializer.deserializeValue("optional", m_optional), babelwires::ParseException);
        };
        babelwires::ShortId m_optional;
    };

    struct SerializableOptional_Activate : SerializableOptional {
        SERIALIZABLE(SerializableOptional_Activate, "activate", SerializableOptional, 1);
    };
    struct SerializableOptional_Deactivate : SerializableOptional {
        SERIALIZABLE(SerializableOptional_Deactivate, "deactivate", SerializableOptional, 1);
    };
}

void babelwires::SelectOptionalsModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    std::vector<std::unique_ptr<SerializableOptional>> temp;
    for (const auto& [optional, isActivated] : m_optionalsActivation) {
        if (isActivated) {
            temp.emplace_back(std::make_unique<SerializableOptional_Activate>())->m_optional = optional;
        } else {
            temp.emplace_back(std::make_unique<SerializableOptional_Deactivate>())->m_optional = optional;
        }
    }
    serializer.serializeArray("optionals", temp);
}

void babelwires::SelectOptionalsModifierData::deserializeContents(Deserializer& deserializer) {
    THROW_ON_ERROR(deserializer.deserializeValue("path", m_targetPath), ParseException);
    if (auto itResult = deserializer.tryDeserializeArray<SerializableOptional>("optionals")) {
        for (auto& it = *itResult; it.isValid(); ++it) {
            const auto newObject = it.getObject();
            if (newObject->tryAs<SerializableOptional_Activate>()) {
                m_optionalsActivation[newObject->m_optional] = true;
            } else if (newObject->tryAs<SerializableOptional_Deactivate>()) {
                m_optionalsActivation[newObject->m_optional] = false;
            } else {
                throw ModelException() << "Problem deserializing activated/deactivated optional";
            }
        }
    }
}

void babelwires::SelectOptionalsModifierData::apply(ValueTreeNode* target) const {
    if (auto recordType = target->getType()->tryAs<RecordType>()) {
        const TypeSystem& typeSystem = target->getTypeSystem();
        ValueHolder newValue = target->getValue();
        recordType->selectOptionals(typeSystem, newValue, m_optionalsActivation);
        target->setValue(newValue);
        return;
    }
    throw ModelException() << "Cannot activate optionals from a value which does not have optionals";
}

void babelwires::SelectOptionalsModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    // Need to be careful with the map, as visiting may change the identifiers.
    std::vector<std::pair<ShortId, bool>> temp(m_optionalsActivation.begin(), m_optionalsActivation.end());
    for (auto& f : temp) {
        visitor(f.first);
    }
    m_optionalsActivation.clear();
    for (const auto& f : temp) {
        m_optionalsActivation[f.first] = f.second;
    }
}

void babelwires::SelectOptionalsModifierData::setOptionalActivation(ShortId optional, bool isActivate) {
    m_optionalsActivation[optional] = isActivate;
} 

void babelwires::SelectOptionalsModifierData::resetOptionalActivation(ShortId optional) {
    m_optionalsActivation.erase(optional);
}

bool babelwires::SelectOptionalsModifierData::isDefaultState() const {
    return getOptionalActivationData().empty();
}

const std::map<babelwires::ShortId, bool>& babelwires::SelectOptionalsModifierData::getOptionalActivationData() const {
    return m_optionalsActivation;
}
