/**
 * A Modifier changes the value in a Node, and corresponds to a user edit.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Common/Log/userLogger.hpp>

#include <cassert>

babelwires::Modifier::Modifier(std::unique_ptr<ModifierData> moddata)
    : m_data(std::move(moddata)) {
    assert(m_data && "moddata must be non-null");
}

babelwires::Modifier::Modifier(const Modifier& other)
    : m_reasonForFailure(other.m_reasonForFailure)
    , m_data(other.m_data->clone())
    , m_state(other.m_state)
    , m_changes(other.m_changes) {}

babelwires::Modifier::~Modifier() = default;

const babelwires::Node* babelwires::Modifier::getOwner() const {
    return m_owner;
}

babelwires::Node* babelwires::Modifier::getOwner() {
    return m_owner;
}

void babelwires::Modifier::setOwner(Node* newOwner) {
    assert((!m_owner || !newOwner) && "A modifier cannot be given an owner when it already has an owner");
    m_owner = newOwner;
}

babelwires::ModifierData& babelwires::Modifier::getModifierData() {
    return *m_data;
}

const babelwires::ModifierData& babelwires::Modifier::getModifierData() const {
    return *m_data;
}

const babelwires::Path& babelwires::Modifier::getTargetPath() const {
    return m_data->m_targetPath;
}

void babelwires::Modifier::unapply(ValueTreeNode* container) const {
    assert(!isFailed() && "Don't try to unapply a failed modifier.");
    try {
        ValueTreeNode& f = followPath(m_data->m_targetPath, *container);
        // When undone, array size modifiers should not reset the remaining children.
        f.setToDefault();
    } catch (const BaseException& e) {
        assert(!"It should always be possible to unapply a modifier.");
    }
}

void babelwires::Modifier::applyIfLocal(UserLogger& userLogger, ValueTreeNode* container) {}

bool babelwires::Modifier::isChanged(Changes changes) const {
    return (m_changes & changes) != Changes::NothingChanged;
}

void babelwires::Modifier::clearChanges() {
    m_changes = Changes::NothingChanged;
}

void babelwires::Modifier::setChanged(Changes changes) {
    m_changes = m_changes | changes;
}

babelwires::Modifier::State babelwires::Modifier::getState() const {
    assert((m_reasonForFailure.empty() ^ (m_state != State::Success)) && "Inconsistent reason and state");
    return m_state;
}

std::string babelwires::Modifier::getReasonForFailure() const {
    assert((m_reasonForFailure.empty() ^ (m_state != State::Success)) && "Inconsistent reason and state");
    return m_reasonForFailure;
}

void babelwires::Modifier::setSucceeded() {
    if (m_state != State::Success) {
        Changes changes = Changes::ModifierRecovered;
        Node::Changes elementChanges = Node::Changes::ModifierRecovered;
        if (m_state != State::ApplicationFailed) {
            changes = changes | Changes::ModifierConnected;
            elementChanges = elementChanges | Node::Changes::ModifierConnected;
        }
        setChanged(changes);
        m_owner->setChanged(elementChanges);
        m_reasonForFailure.clear();
        m_state = State::Success;
    }
}

void babelwires::Modifier::setFailed(State failureState, std::string reasonForFailure) {
    assert((failureState != State::Success) && "You can't set failed with success");
    if (m_state == State::Success) {
        Changes changes = Changes::ModifierFailed;
        Node::Changes elementChanges = Node::Changes::ModifierFailed;
        if (failureState != State::ApplicationFailed) {
            changes = changes | Changes::ModifierDisconnected;
            elementChanges = elementChanges | Node::Changes::ModifierDisconnected;
        }
        setChanged(changes);
        m_owner->setChanged(elementChanges);
    }
    m_state = failureState;
    m_reasonForFailure = std::move(reasonForFailure);
}

void babelwires::Modifier::adjustArrayIndex(const babelwires::Path& pathToArray, babelwires::ArrayIndex startIndex,
                                            int adjustment) {
    babelwires::ModifierData& modifierData = getModifierData();
    Path& modifierPath = modifierData.m_targetPath;

    assert(pathToArray.isStrictPrefixOf(modifierPath) && "This code only applies when the path is correct.");
    const unsigned int pathIndexOfStepIntoArray = pathToArray.getNumSteps();
    babelwires::PathStep& stepIntoArray = modifierPath.getStep(pathIndexOfStepIntoArray);
    auto index = stepIntoArray.getIndex();
    assert((index >= startIndex) && "This code only applies when the index is correct.");
    if (!isChanged(Changes::ModifierMoved)) {
        setChanged(Changes::ModifierMoved);
        getOwner()->setModifierMoving(*this);
    }
    stepIntoArray = index + adjustment;
}

const babelwires::ConnectionModifier* babelwires::Modifier::asConnectionModifier() const {
    return doAsConnectionModifier();
}

babelwires::ConnectionModifier* babelwires::Modifier::asConnectionModifier() {
    return const_cast<babelwires::ConnectionModifier*>(doAsConnectionModifier());
}
