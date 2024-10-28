/**
 * A ConnectionModifier connects features from two FeatureElements together.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <Common/Log/userLogger.hpp>

#include <cassert>

babelwires::ConnectionModifier::ConnectionModifier(std::unique_ptr<ConnectionModifierData> moddata)
    : Modifier(std::move(moddata)) {}

babelwires::ConnectionModifier::ConnectionModifier(const ConnectionModifier& other)
    : Modifier(other) {}

const babelwires::ConnectionModifierData& babelwires::ConnectionModifier::getModifierData() const {
    assert(Modifier::getModifierData().as<babelwires::ConnectionModifierData>() &&
           "Holding wrong kind of data");
    return static_cast<const babelwires::ConnectionModifierData&>(Modifier::getModifierData());
}

babelwires::ConnectionModifierData& babelwires::ConnectionModifier::getModifierData() {
    assert(Modifier::getModifierData().as<babelwires::ConnectionModifierData>() &&
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

    try {
        const babelwires::ConnectionModifierData& data = getModifierData();
        target = data.getTarget(container);
        state = State::SourceMissing;
        const ValueTreeNode* source = data.getSourceTreeNode(project);
        state = State::ApplicationFailed;
        data.apply(source, target, shouldForce);
        setSucceeded();
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to apply operation: " << e.what();
        if (target) {
            target->setToDefault();
        }
        setFailed(state, e.what());
    }
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
