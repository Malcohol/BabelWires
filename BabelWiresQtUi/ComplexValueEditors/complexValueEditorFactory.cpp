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

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/Types/Map/SumOfMaps/sumOfMapsType.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

babelwires::ComplexValueEditor* babelwires::ComplexValueEditorFactory::createEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ProjectDataLocation& data) {
    AccessModelScope scope(projectBridge);
    const ValueTreeNode& valueTreeNode = ComplexValueEditor::getValueTreeNodeOrThrow(scope, data);
    const Type& type = valueTreeNode.getType();

    // TODO: For now, assume ComplexValueEditors are all built-in, so we don't need a registry.
    if (type.as<MapType>() || type.as<SumOfMapsType>()) {
        // TODO: For now use a floating window.
        return new MapEditor(nullptr, projectBridge, userLogger, data);
    }

    throw ModelException() << "There is no known editor for that type of value.";
}
