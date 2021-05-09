/**
 * A ConnectionModifier connects features from two FeatureElements together.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/Modifiers/connectionModifier.hpp"

#include "BabelWires/Features/modelExceptions.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"

#include "Common/Log/userLogger.hpp"

#include <cassert>

babelwires::ConnectionModifier::ConnectionModifier(std::unique_ptr<ConnectionModifierData> moddata)
    : Modifier(std::move(moddata)) {}

babelwires::ConnectionModifier::ConnectionModifier(const ConnectionModifier& other)
    : Modifier(other) {}

const babelwires::ConnectionModifierData& babelwires::ConnectionModifier::getModifierData() const {
    assert(dynamic_cast<const babelwires::ConnectionModifierData*>(&Modifier::getModifierData()) &&
           "Holding wrong kind of data");
    return static_cast<const babelwires::ConnectionModifierData&>(Modifier::getModifierData());
}

babelwires::ConnectionModifierData& babelwires::ConnectionModifier::getModifierData() {
    assert(dynamic_cast<const babelwires::ConnectionModifierData*>(&Modifier::getModifierData()) &&
           "Holding wrong kind of data");
    return static_cast<babelwires::ConnectionModifierData&>(Modifier::getModifierData());
}

const babelwires::ConnectionModifier* babelwires::ConnectionModifier::doAsConnectionModifier() const {
    return this;
}

void babelwires::ConnectionModifier::applyConnection(const Project& project, UserLogger& userLogger,
                                                     Feature* container) {
    // Force the application if the modifier is new OR is currently failed, in case it now succeeds.
    const bool shouldForce = isChanged(Changes::ModifierIsNew) || isFailed();

    State state = State::TargetMissing;
    Feature* targetFeature = nullptr;

    try {
        const babelwires::ConnectionModifierData& data = getModifierData();
        targetFeature = data.getTargetFeature(container);
        state = State::SourceMissing;
        const Feature* sourceFeature = data.getSourceFeature(project);
        state = State::ApplicationFailed;
        data.apply(sourceFeature, targetFeature, shouldForce);
        setSucceeded();
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to apply operation: " << e.what();
        if (targetFeature) {
            targetFeature->setToDefault();
        }
        setFailed(state, e.what());
    }
}

bool babelwires::ConnectionModifier::isConnected() const {
    const State state = getState();
    return ((state == State::Success) || (state == State::ApplicationFailed));
}

void babelwires::ConnectionModifier::adjustSourceArrayIndices(const babelwires::FeaturePath& pathToArray,
                                                              babelwires::ArrayIndex startIndex, int adjustment) {
    babelwires::ConnectionModifierData& modifierData = getModifierData();
    babelwires::FeaturePath& modifierPath = modifierData.m_pathToSourceFeature;
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
