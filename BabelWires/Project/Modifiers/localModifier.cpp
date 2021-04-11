/**
 * A LocalModifier is a modifier which operates on the contents of a single FeatureElement, and is unaffected by context.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/Modifiers/localModifier.hpp"

#include "BabelWires/Features/modelExceptions.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"

#include "Common/Log/userLogger.hpp"

#include <cassert>

babelwires::LocalModifier::LocalModifier(std::unique_ptr<LocalModifierData> moddata)
    : Modifier(std::move(moddata)) {}

babelwires::LocalModifier::LocalModifier(const LocalModifier& other)
    : Modifier(other) {}

const babelwires::LocalModifierData& babelwires::LocalModifier::getModifierData() const {
    return dynamic_cast<const babelwires::LocalModifierData&>(Modifier::getModifierData());
}

babelwires::LocalModifierData& babelwires::LocalModifier::getModifierData() {
    return dynamic_cast<babelwires::LocalModifierData&>(Modifier::getModifierData());
}

const babelwires::ConnectionModifier* babelwires::LocalModifier::doAsConnectionModifier() const {
    return nullptr;
}

void babelwires::LocalModifier::applyIfLocal(UserLogger& userLogger, Feature* container) {
    State state = State::TargetMissing;
    Feature* targetFeature = nullptr;
    try {
        const LocalModifierData& data = getModifierData();
        targetFeature = data.getTargetFeature(container);
        state = State::ApplicationFailed;
        data.apply(targetFeature);
        setSucceeded();
    } catch (const BaseException& e) {
        userLogger.logError() << "Failed to apply operation: " << e.what();
        if (targetFeature) {
            targetFeature->setToDefault();
        }
        setFailed(state, e.what());
    }
}
