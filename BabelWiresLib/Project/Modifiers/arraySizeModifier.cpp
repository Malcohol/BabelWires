/**
 * ArraySizeModifier extends local modifier with some array specific methods.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Log/userLogger.hpp>

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
    try {
        ArraySizeModifierData& data = getModifierData();
        ValueTreeNode* target = data.getTarget(container);
        state = State::ApplicationFailed;
        data.addEntries(target, indexOfNewElement, numEntriesToAdd);
        setSucceeded();
        return true;
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to add entries to an array: " << e.what();
        setFailed(state, e.what());
        return false;
    }
}

bool babelwires::ArraySizeModifier::removeArrayEntries(UserLogger& userLogger, ValueTreeNode* container,
                                                       int indexOfElementToRemove, int numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    State state = State::TargetMissing;
    try {
        ArraySizeModifierData& data = getModifierData();
        ValueTreeNode* target = data.getTarget(container);
        state = State::ApplicationFailed;
        data.removeEntries(target, indexOfElementToRemove, numEntriesToRemove);
        setSucceeded();
        return true;
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to remove entries from an array: " << e.what();
        setFailed(state, e.what());
        return false;
    }
}
