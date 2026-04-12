/**
 * A LocalModifier is a modifier which operates on the contents of a single Node, and is unaffected by context.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Log/userLogger.hpp>

#include <cassert>

babelwires::LocalModifier::LocalModifier(std::unique_ptr<LocalModifierData> moddata)
    : Modifier(std::move(moddata)) {}

babelwires::LocalModifier::LocalModifier(const LocalModifier& other)
    : Modifier(other) {}

const babelwires::LocalModifierData& babelwires::LocalModifier::getModifierData() const {
    return *Modifier::getModifierData().tryAs<babelwires::LocalModifierData>();
}

babelwires::LocalModifierData& babelwires::LocalModifier::getModifierData() {
    return *Modifier::getModifierData().tryAs<babelwires::LocalModifierData>();
}

const babelwires::ConnectionModifier* babelwires::LocalModifier::doAsConnectionModifier() const {
    return nullptr;
}

void babelwires::LocalModifier::applyIfLocal(UserLogger& userLogger, ValueTreeNode* container) {
    State state = State::TargetMissing;
    const LocalModifierData& data = getModifierData();
    const auto targetResult = data.getTarget(container);
    if (!targetResult) {
        userLogger.logError() << "Failed to apply operation: " << targetResult.error().toString();
        setFailed(state, targetResult.error().toString());
        return;
    }
    ValueTreeNode& target = *targetResult;
    state = State::ApplicationFailed;
    const auto result = data.apply(&target);
    if (!result) {
        userLogger.logError() << "Failed to apply operation: " << result.error().toString();
        target.setToDefault();
        setFailed(state, result.error().toString());
        return;
    }
    setSucceeded();
}
