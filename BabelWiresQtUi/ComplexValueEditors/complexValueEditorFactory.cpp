/**
 * A ComplexValueEditorFactory creates the appropriate ComplexValueEditor for a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditorFactory.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/Types/Map/SumOfMaps/sumOfMapsType.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

babelwires::ComplexValueEditor* babelwires::ComplexValueEditorFactory::createEditor(QWidget* parent, ProjectGraphModel& projectGraphModel, UserLogger& userLogger, const ProjectDataLocation& data) {
    AccessModelScope scope(projectGraphModel);
    const ValueTreeNode& valueTreeNode = ComplexValueEditor::getValueTreeNodeOrThrow(scope, data);
    const Type& type = *valueTreeNode.getType();

    // TODO: For now, assume ComplexValueEditors are all built-in, so we don't need a registry.
    if (type.tryAs<MapType>() || type.tryAs<SumOfMapsType>()) {
        // TODO: For now use a floating window.
        return new MapEditor(nullptr, projectGraphModel, userLogger, data);
    }

    throw ModelException() << "There is no known editor for that type of value.";
}
