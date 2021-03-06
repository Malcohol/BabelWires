/**
 * A ComplexValueEditorFactory creates the appropriate ComplexValueEditor for a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditorFactory.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

babelwires::ComplexValueEditor* babelwires::ComplexValueEditorFactory::createEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data) {
    AccessModelScope scope(projectBridge);
    const ValueFeature& valueFeature = ComplexValueEditor::getValueFeatureOrThrow(scope, data);

    // TODO: For now, assume ComplexValueEditors are all built-in, so we don't need a registry.
    if (valueFeature.as<MapFeature>()) {
        // TODO: For now use a floating window.
        return new MapEditor(nullptr, projectBridge, userLogger, data);
    }

    throw ModelException() << "There is no known editor for that type of value.";
}
