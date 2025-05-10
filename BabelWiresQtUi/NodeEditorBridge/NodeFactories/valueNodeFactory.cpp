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

#include <optional>

babelwires::ValueNodeFactory::ValueNodeFactory(const TypeSystem& typeSystem)
    : m_typeSystem(typeSystem) {}

QString babelwires::ValueNodeFactory::getCategoryName() const {
    return "Values";
}

QList<QString> babelwires::ValueNodeFactory::getFactoryNames() const {
    QList<QString> factoryNames;
    auto identifierRegistry = IdentifierRegistry::read();
    for (const auto& typeId : m_typeSystem.getAllPrimitiveTypes()) {
        factoryNames.append(identifierRegistry->getName(typeId).c_str());
    }
    return factoryNames;
}

void babelwires::ValueNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
                                              QPointF scenePos) {
    std::optional<PrimitiveTypeId> typeId;
    {
        auto identifierRegistry = IdentifierRegistry::read();
        for (const auto& tid : m_typeSystem.getAllPrimitiveTypes()) {
            if(factoryName == identifierRegistry->getName(tid).c_str()) {
                typeId = tid;
                break;
            }
        }
        assert(typeId && "FactoryName not found when creating Value Node");
    }

    auto newNodeData = std::make_unique<ValueNodeData>(*typeId);
    newNodeData->m_uiData.m_uiPosition.m_x = scenePos.x();
    newNodeData->m_uiData.m_uiPosition.m_y = scenePos.y();

    projectGraphModel.scheduleCommand(std::make_unique<AddNodeCommand>("Add Value Node", std::move(newNodeData)));
}
