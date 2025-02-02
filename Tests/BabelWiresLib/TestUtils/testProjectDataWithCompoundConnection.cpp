#include <Tests/BabelWiresLib/TestUtils/testProjectDataWithCompoundConnection.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>


testUtils::TestProjectDataWithCompoundConnection::TestProjectDataWithCompoundConnection() {
    testUtils::TestComplexRecordElementData sourceRecordNodeData;
    sourceRecordNodeData.m_id = m_sourceNodeId;

    auto sourceIntModifier = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(-14));
    sourceIntModifier->m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt0();
    sourceRecordNodeData.m_modifiers.emplace_back(std::move(sourceIntModifier));

    sourceRecordNodeData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord());

    m_nodes.emplace_back(sourceRecordNodeData.clone());

    testUtils::TestComplexRecordElementData targetRecordNodeData;
    targetRecordNodeData.m_id = m_targetNodeId;

    auto targetIntModifier = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(87));
    targetIntModifier->m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
    targetRecordNodeData.m_modifiers.emplace_back(std::move(targetIntModifier));

    auto connection = std::make_unique<babelwires::ConnectionModifierData>();
    connection->m_sourceId = m_sourceNodeId;
    connection->m_sourcePath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecord();
    connection->m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecord();

    targetRecordNodeData.m_modifiers.emplace_back(std::move(connection));

    targetRecordNodeData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord());

    m_nodes.emplace_back(targetRecordNodeData.clone());
}
