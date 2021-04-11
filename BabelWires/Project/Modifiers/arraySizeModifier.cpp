/**
 * ArraySizeModifier extends local modifier with some array specific methods.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/Modifiers/arraySizeModifier.hpp"

#include "BabelWires/Features/modelExceptions.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"

#include "Common/Log/userLogger.hpp"

#include <cassert>

babelwires::ArraySizeModifier::ArraySizeModifier(std::unique_ptr<ArrayInitializationData> moddata)
    : LocalModifier(std::move(moddata)) {}

babelwires::ArraySizeModifier::ArraySizeModifier(const ArraySizeModifier& other)
    : LocalModifier(other) {}

const babelwires::ArrayInitializationData& babelwires::ArraySizeModifier::getModifierData() const {
    return dynamic_cast<const babelwires::ArrayInitializationData&>(Modifier::getModifierData());
}

babelwires::ArrayInitializationData& babelwires::ArraySizeModifier::getModifierData() {
    return dynamic_cast<babelwires::ArrayInitializationData&>(Modifier::getModifierData());
}

bool babelwires::ArraySizeModifier::addArrayEntries(UserLogger& userLogger, Feature* container, int indexOfNewElement,
                                                    int numEntriesToAdd) {
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");
    State state = State::TargetMissing;
    try {
        ArrayInitializationData& data = getModifierData();
        Feature* targetFeature = data.getTargetFeature(container);
        state = State::ApplicationFailed;
        data.addEntries(targetFeature, indexOfNewElement, numEntriesToAdd);
        setSucceeded();
        return true;
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to add entries to an array: " << e.what();
        setFailed(state, e.what());
        return false;
    }
}

bool babelwires::ArraySizeModifier::removeArrayEntries(UserLogger& userLogger, Feature* container,
                                                       int indexOfElementToRemove, int numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    State state = State::TargetMissing;
    try {
        ArrayInitializationData& data = getModifierData();
        Feature* targetFeature = data.getTargetFeature(container);
        state = State::ApplicationFailed;
        data.removeEntries(targetFeature, indexOfElementToRemove, numEntriesToRemove);
        setSucceeded();
        return true;
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to remove entries from an array: " << e.what();
        setFailed(state, e.what());
        return false;
    }
}
