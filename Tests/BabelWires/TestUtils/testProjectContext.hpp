#pragma once

#include "Tests/TestUtils/testDeserializationRegistry.hpp"
#include "Tests/TestUtils/testLog.hpp"

#include "BabelWires/FileFormat/fileFormat.hpp"
#include "BabelWires/Processors/processorFactoryRegistry.hpp"
#include "BabelWires/Project/project.hpp"
#include "BabelWires/Project/projectContext.hpp"

namespace libTestUtils {
    struct TestProjectContext {
        TestProjectContext();

        babelwires::FileFeatureFactoryRegistry m_factoryFormatReg;
        babelwires::FileFormatRegistry m_fileFormatReg;
        babelwires::ProcessorFactoryRegistry m_processorReg;
        testUtils::TestDeserializationRegistry m_deserializationReg;

        babelwires::ProjectContext m_projectContext;
        testUtils::TestLogWithListener m_log;

        babelwires::Project m_project;
    };
} // namespace libTestUtils
