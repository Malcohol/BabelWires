/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

#include <QCloseEvent>

babelwires::ComplexValueEditor::ComplexValueEditor(QWidget* parent, ProjectBridge& projectBridge,
                                                   UserLogger& userLogger, const DataLocation& data)
    : QWidget(parent)
    , m_projectBridge(projectBridge)
    , m_userLogger(userLogger)
    , m_data(data) {}

const babelwires::DataLocation& babelwires::ComplexValueEditor::getData() const {
    return m_data;
}

babelwires::ProjectBridge& babelwires::ComplexValueEditor::getProjectBridge() {
    return m_projectBridge;
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
babelwires::ComplexValueEditor::getValueTreeNodeOrThrow(AccessModelScope& scope, const DataLocation& data) {
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.getElementId());

    if (!element) {
        throw ModelException() << "The element does not exist.";
    }

    const ValueTreeNode* const input = element->getInput();
    if (!input) {
        throw ModelException() << "The element does not have editable features.";
    }

    const auto* const inputTreeNode = data.getPathToValue().tryFollow(*input);
    if (!inputTreeNode) {
        throw ModelException() << "There is no value at that location.";
    }
    return *inputTreeNode;
}

const babelwires::ValueTreeNode* babelwires::ComplexValueEditor::tryGetValueTreeNode(AccessModelScope& scope,
                                                                                   const DataLocation& data) {
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.getElementId());

    if (!element) {
        return nullptr;
    }

    const ValueTreeNode* const input = element->getInput();
    if (!input) {
        return nullptr;
    }

    const auto* const inputTreeNode = data.getPathToValue().tryFollow(*input);
    if (!inputTreeNode) {
        return nullptr;
    }
    return inputTreeNode;
}

const babelwires::ValueTreeNode& babelwires::ComplexValueEditor::getValueTreeNode(AccessModelScope& scope,
                                                                                const DataLocation& data) {
    const ValueTreeNode* const valueTreeNode = tryGetValueTreeNode(scope, data);
    assert(valueTreeNode && "There was not value feature");
    return *valueTreeNode;
}

