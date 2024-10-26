/**
 * A LocalModifier is a modifier which operates on the contents of a single FeatureElement, and is unaffected by context.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <Common/Log/userLogger.hpp>

#include <cassert>

babelwires::LocalModifier::LocalModifier(std::unique_ptr<LocalModifierData> moddata)
    : Modifier(std::move(moddata)) {}

babelwires::LocalModifier::LocalModifier(const LocalModifier& other)
    : Modifier(other) {}

const babelwires::LocalModifierData& babelwires::LocalModifier::getModifierData() const {
    return *Modifier::getModifierData().as<babelwires::LocalModifierData>();
}

babelwires::LocalModifierData& babelwires::LocalModifier::getModifierData() {
    return *Modifier::getModifierData().as<babelwires::LocalModifierData>();
}

const babelwires::ConnectionModifier* babelwires::LocalModifier::doAsConnectionModifier() const {
    return nullptr;
}

void babelwires::LocalModifier::applyIfLocal(UserLogger& userLogger, ValueTreeNode* container) {
    State state = State::TargetMissing;
    ValueTreeNode* targetFeature = nullptr;
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
