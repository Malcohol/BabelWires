/**
 * ArraySizeModifier extends local modifier with some array specific methods.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>

#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BaseLib/Context/context.hpp>

#include <BaseLib/Log/userLogger.hpp>

#include <cassert>

babelwires::ArraySizeModifier::ArraySizeModifier(std::unique_ptr<ArraySizeModifierData> moddata)
    : LocalModifier(std::move(moddata)) {}

babelwires::ArraySizeModifier::ArraySizeModifier(const ArraySizeModifier& other)
    : LocalModifier(other) {}

const babelwires::ArraySizeModifierData& babelwires::ArraySizeModifier::getModifierData() const {
    return *Modifier::getModifierData().tryAs<babelwires::ArraySizeModifierData>();
}

babelwires::ArraySizeModifierData& babelwires::ArraySizeModifier::getModifierData() {
    return *Modifier::getModifierData().tryAs<babelwires::ArraySizeModifierData>();
}

bool babelwires::ArraySizeModifier::addArrayEntries(UserLogger& userLogger, ValueTreeNode* container, int indexOfNewElement,
                                                    int numEntriesToAdd) {
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");
    State state = State::TargetMissing;
    ArraySizeModifierData& data = getModifierData();
    const auto targetResult = data.getTarget(container);
    if (!targetResult) {
        userLogger.logError() << "Failed to add entries to an array: " << targetResult.error().toString();
        setFailed(state, targetResult.error().toString());
        return false;
    }
    ValueTreeNode* target = &*targetResult;
    state = State::ApplicationFailed;
    const auto result = data.addEntries(target, indexOfNewElement, numEntriesToAdd);
    if (!result) {
        userLogger.logError() << "Failed to add entries to an array: " << result.error().toString();
        setFailed(state, result.error().toString());
        return false;
    }
    setSucceeded();
    return true;
}

bool babelwires::ArraySizeModifier::removeArrayEntries(UserLogger& userLogger, ValueTreeNode* container,
                                                       int indexOfElementToRemove, int numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    State state = State::TargetMissing;
    ArraySizeModifierData& data = getModifierData();
    const auto targetResult = data.getTarget(container);
    if (!targetResult) {
        userLogger.logError() << "Failed to remove entries from an array: " << targetResult.error().toString();
        setFailed(state, targetResult.error().toString());
        return false;
    }
    ValueTreeNode* target = &*targetResult;
    state = State::ApplicationFailed;
    const auto result = data.removeEntries(target, indexOfElementToRemove, numEntriesToRemove);
    if (!result) {
        userLogger.logError() << "Failed to remove entries from an array: " << result.error().toString();
        setFailed(state, result.error().toString());
        return false;
    }
    setSucceeded();
    return true;
}
