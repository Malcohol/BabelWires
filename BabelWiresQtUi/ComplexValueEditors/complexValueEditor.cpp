/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
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

const babelwires::ValueFeature&
babelwires::ComplexValueEditor::getValueFeatureOrThrow(AccessModelScope& scope, const DataLocation& data) {
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.getElementId());

    if (!element) {
        throw ModelException() << "The element does not exist.";
    }

    const Feature* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        throw ModelException() << "The element does not have editable features.";
    }

    const auto* const valueFeature = data.getPathToValue().tryFollow(*inputFeature)->as<ValueFeature>();
    if (!valueFeature) {
        throw ModelException() << "There is no value at that location.";
    }
    return *valueFeature;
}

const babelwires::ValueFeature* babelwires::ComplexValueEditor::tryGetValueFeature(AccessModelScope& scope,
                                                                                   const DataLocation& data) {
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.getElementId());

    if (!element) {
        return nullptr;
    }

    const Feature* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        return nullptr;
    }

    const auto* const valueFeature = data.getPathToValue().tryFollow(*inputFeature)->as<ValueFeature>();
    if (!valueFeature) {
        return nullptr;
    }
    return valueFeature;
}

const babelwires::ValueFeature& babelwires::ComplexValueEditor::getValueFeature(AccessModelScope& scope,
                                                                                const DataLocation& data) {
    const ValueFeature* const valueFeature = tryGetValueFeature(scope, data);
    assert(valueFeature && "There was not value feature");
    return *valueFeature;
}

