/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>

#include <QCloseEvent>

babelwires::ComplexValueEditor::ComplexValueEditor(QWidget* parent, ProjectBridge& projectBridge,
                                                   UserLogger& userLogger, const ComplexValueEditorData& data)
    : QWidget(parent)
    , m_projectBridge(projectBridge)
    , m_userLogger(userLogger)
    , m_data(data) {}

const babelwires::ComplexValueEditorData& babelwires::ComplexValueEditor::getData() const {
    return m_data;
}

babelwires::ProjectBridge& babelwires::ComplexValueEditor::getProjectBridge() {
    return m_projectBridge;
}

void babelwires::ComplexValueEditor::closeEvent(QCloseEvent* event) {
    emit editorClosing();
    event->accept();
}

const babelwires::ValueFeature& babelwires::ComplexValueEditor::getValueFeature(AccessModelScope& scope, const ComplexValueEditorData& data) {
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.getElementId());

    if (!element) {
        throw ModelException() << "The element does not exist.";
    }

    const Feature* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        throw ModelException() << "The element does not have editable features.";
    }

    const ValueFeature* const valueFeature = data.getPathToValue().tryFollow(*inputFeature)->as<ValueFeature>();
    if (!valueFeature) {
        throw ModelException() << "There is no value at that location.";
    }
    return *valueFeature;
}

const babelwires::ValueFeature* babelwires::ComplexValueEditor::tryGetValueFeature(AccessModelScope& scope, const ComplexValueEditorData& data) {
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.getElementId());

    if (!element) {
        return nullptr;
    }

    const Feature* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        return nullptr;
    }

    const ValueFeature* const valueFeature = data.getPathToValue().tryFollow(*inputFeature)->as<ValueFeature>();
    if (!valueFeature) {
        return nullptr;
    }
    return valueFeature;  
}
