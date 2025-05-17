/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <QCloseEvent>

babelwires::ComplexValueEditor::ComplexValueEditor(QWidget* parent, ProjectGraphModel& projectGraphModel,
                                                   UserLogger& userLogger, const ProjectDataLocation& data)
    : QWidget(parent)
    , m_projectGraphModel(projectGraphModel)
    , m_userLogger(userLogger)
    , m_data(data.clone()) {}

const babelwires::ProjectDataLocation& babelwires::ComplexValueEditor::getDataLocation() const {
    return *m_data;
}

babelwires::ProjectGraphModel& babelwires::ComplexValueEditor::getProjectGraphModel() {
    return m_projectGraphModel;
}

babelwires::UserLogger& babelwires::ComplexValueEditor::getUserLogger() {
    return m_userLogger;
}

void babelwires::ComplexValueEditor::closeEvent(QCloseEvent* event) {
    if (maybeApplyToProject()) {
        emit editorClosing();
        getUserLogger().logInfo() << "Close map editor";
        event->accept();
    } else {
        event->ignore();
    }
}

const babelwires::ValueTreeNode&
babelwires::ComplexValueEditor::getValueTreeNodeOrThrow(const AccessModelScope& scope, const ProjectDataLocation& data) {
    const Project& project = scope.getProject();

    const Node* node = project.getNode(data.getNodeId());

    if (!node) {
        throw ModelException() << "The node does not exist.";
    }

    const ValueTreeNode* const input = node->getInput();
    if (!input) {
        throw ModelException() << "The node does not have editable content.";
    }

    const auto* const inputTreeNode = tryFollowPath(data.getPathToValue(), *input);
    if (!inputTreeNode) {
        throw ModelException() << "There is no value at that location.";
    }
    return *inputTreeNode;
}

const babelwires::ValueTreeNode* babelwires::ComplexValueEditor::tryGetValueTreeNode(const AccessModelScope& scope,
                                                                                   const ProjectDataLocation& data) {
    const Project& project = scope.getProject();

    const Node* node = project.getNode(data.getNodeId());

    if (!node) {
        return nullptr;
    }

    const ValueTreeNode* const input = node->getInput();
    if (!input) {
        return nullptr;
    }

    const auto* const inputTreeNode = tryFollowPath(data.getPathToValue(), *input);
    if (!inputTreeNode) {
        return nullptr;
    }
    return inputTreeNode;
}

const babelwires::ValueTreeNode& babelwires::ComplexValueEditor::getValueTreeNode(const AccessModelScope& scope,
                                                                                const ProjectDataLocation& data) {
    const ValueTreeNode* const valueTreeNode = tryGetValueTreeNode(scope, data);
    assert(valueTreeNode && "There was no ValueTreeNode");
    return *valueTreeNode;
}

