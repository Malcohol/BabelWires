/**
 * A ComplexValueEditorFactory creates the appropriate ComplexValueEditor for a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditorFactory.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/mapEditor.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

babelwires::ComplexValueEditor* babelwires::ComplexValueEditorFactory::createEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data) {
    AccessModelScope scope(projectBridge);
    const Project& project = scope.getProject();

    const FeatureElement* element = project.getFeatureElement(data.m_elementId);

    if (!element) {
        throw ModelException() << "The element does not exist.";
    }

    const Feature* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        throw ModelException() << "The element does not have editable features.";
    }

    const ValueFeature* const valueFeature = data.m_pathToValue.tryFollow(*inputFeature)->as<ValueFeature>();
    if (!valueFeature) {
        throw ModelException() << "There is no value at that location.";
    }

    if (valueFeature->as<MapFeature>()) {
        // TODO: For now use a floating window.
        return new MapEditor(nullptr, projectBridge, userLogger, data);
    }

    throw ModelException() << "There is no known editor for that type of value.";
}
