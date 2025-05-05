/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/valueNodeFactory.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::ValueNodeFactory::ValueNodeFactory(const TypeSystem& typeSystem)
    : m_typeSystem(typeSystem) {}

QString babelwires::ValueNodeFactory::getCategoryName() const {
    return "Values";
}

QList<QString> babelwires::ValueNodeFactory::getFactoryNames() const {
    for (const auto& t : context.m_typeSystem.getAllPrimitiveTypes()) {
        // TODO
    }
}

void babelwires::ValueNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
                                              QPointF const scenePos, QWidget* parentForDialogs) {
    PrimitiveTypeId typeId;
    for (const auto& t : context.m_typeSystem.getAllPrimitiveTypes()) {
        // TODO
    }

    typeId.resolve(m_typeSystem);

    auto newDataPtr = std::make_unique<ValueNodeData>(m_typeOfValue);
    // TODO position

    projectGraphModel->scheduleCommand(std::make_unique<AddNodeCommand>("Add Value Node", std::move(newDataPtr)));
}
