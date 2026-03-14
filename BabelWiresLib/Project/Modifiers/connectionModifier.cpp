/**
 * A ConnectionModifier connects features from two Nodes together.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <BaseLib/Log/userLogger.hpp>

#include <cassert>

babelwires::ConnectionModifier::ConnectionModifier(std::unique_ptr<ConnectionModifierData> moddata)
    : Modifier(std::move(moddata)) {}

babelwires::ConnectionModifier::ConnectionModifier(const ConnectionModifier& other)
    : Modifier(other) {}

const babelwires::ConnectionModifierData& babelwires::ConnectionModifier::getModifierData() const {
    assert(Modifier::getModifierData().tryAs<babelwires::ConnectionModifierData>() &&
           "Holding wrong kind of data");
    return static_cast<const babelwires::ConnectionModifierData&>(Modifier::getModifierData());
}

babelwires::ConnectionModifierData& babelwires::ConnectionModifier::getModifierData() {
    assert(Modifier::getModifierData().tryAs<babelwires::ConnectionModifierData>() &&
           "Holding wrong kind of data");
    return static_cast<babelwires::ConnectionModifierData&>(Modifier::getModifierData());
}

const babelwires::ConnectionModifier* babelwires::ConnectionModifier::doAsConnectionModifier() const {
    return this;
}

void babelwires::ConnectionModifier::applyConnection(const Project& project, UserLogger& userLogger,
                                                     ValueTreeNode* container, bool shouldForce) {
    // Force the application if the modifier is new OR is currently failed, in case it now succeeds.
    shouldForce = shouldForce || isChanged(Changes::ModifierIsNew) || isFailed();

    State state = State::TargetMissing;
    ValueTreeNode* target = nullptr;

    const babelwires::ConnectionModifierData& data = getModifierData();
    const auto targetResult = data.getTarget(container);
    if (!targetResult) {
        userLogger.logError() << "Failed to apply operation: " << targetResult.error().toString();
        setFailed(state, targetResult.error().toString());
        return;
    }
    target = &*targetResult;
    state = State::SourceMissing;
    const auto sourceResult = data.getSourceTreeNode(project);
    if (!sourceResult) {
        userLogger.logError() << "Failed to apply operation: " << sourceResult.error().toString();
        if (target) {
            target->setToDefault();
        }
        setFailed(state, sourceResult.error().toString());
        return;
    }
    const ValueTreeNode* source = &*sourceResult;
    state = State::ApplicationFailed;
    const auto applyResult = data.apply(source, target, shouldForce);
    if (!applyResult) {
        userLogger.logError() << "Failed to apply operation: " << applyResult.error().toString();
        if (target) {
            target->setToDefault();
        }
        setFailed(state, applyResult.error().toString());
        return;
    }
    setSucceeded();
}

bool babelwires::ConnectionModifier::isConnected() const {
    const State state = getState();
    return ((state == State::Success) || (state == State::ApplicationFailed));
}

void babelwires::ConnectionModifier::adjustSourceArrayIndices(const babelwires::Path& pathToArray,
                                                              babelwires::ArrayIndex startIndex, int adjustment) {
    babelwires::ConnectionModifierData& modifierData = getModifierData();
    babelwires::Path& modifierPath = modifierData.m_sourcePath;
    if (pathToArray.isStrictPrefixOf(modifierPath)) {
        // Is the modifier affected?
        const unsigned int pathIndexOfStepIntoArray = pathToArray.getNumSteps();
        babelwires::PathStep& stepIntoArray = modifierPath.getStep(pathIndexOfStepIntoArray);
        if (stepIntoArray.isIndex()) {
            const babelwires::ArrayIndex arrayIndex = stepIntoArray.getIndex();
            if (arrayIndex >= startIndex) {
                if (!isChanged(Changes::ModifierMoved)) {
                    setChanged(Changes::ModifierMoved);
                    getOwner()->setModifierMoving(*this);
                }
                stepIntoArray = arrayIndex + adjustment;
            }
        }
    }
}
