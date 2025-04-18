#include <Tests/BabelWiresLib/TestUtils/testProjectDataWithCompoundConnection.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>


testUtils::TestProjectDataWithCompoundConnection::TestProjectDataWithCompoundConnection() {
    testDomain::TestComplexRecordElementData sourceRecordNodeData;
    sourceRecordNodeData.m_id = m_sourceNodeId;

    auto sourceIntModifier = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(-14));
    sourceIntModifier->m_targetPath = testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt0();
    sourceRecordNodeData.m_modifiers.emplace_back(std::move(sourceIntModifier));

    sourceRecordNodeData.m_expandedPaths.emplace_back(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());

    m_nodes.emplace_back(sourceRecordNodeData.clone());

    testDomain::TestComplexRecordElementData targetRecordNodeData;
    targetRecordNodeData.m_id = m_targetNodeId;

    auto targetIntModifier = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(87));
    targetIntModifier->m_targetPath = testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
    targetRecordNodeData.m_modifiers.emplace_back(std::move(targetIntModifier));

    auto connection = std::make_unique<babelwires::ConnectionModifierData>();
    connection->m_sourceId = m_sourceNodeId;
    connection->m_sourcePath = testDomain::TestComplexRecordElementData::getPathToRecordSubrecord();
    connection->m_targetPath = testDomain::TestComplexRecordElementData::getPathToRecordSubrecord();

    targetRecordNodeData.m_modifiers.emplace_back(std::move(connection));

    targetRecordNodeData.m_expandedPaths.emplace_back(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());

    m_nodes.emplace_back(targetRecordNodeData.clone());
}
