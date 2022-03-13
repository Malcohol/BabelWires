#pragma once

#include "Tests/TestUtils/testDeserializationRegistry.hpp"
#include "Tests/TestUtils/testLog.hpp"

#include "BabelWiresLib/FileFormat/sourceFileFormat.hpp"
#include "BabelWiresLib/FileFormat/targetFileFormat.hpp"
#include "BabelWiresLib/Processors/processorFactoryRegistry.hpp"
#include "BabelWiresLib/Enums/enum.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"

namespace testUtils {
    struct TestEnvironment {
        TestEnvironment();

        babelwires::TargetFileFormatRegistry m_targetFileFormatReg;
        babelwires::SourceFileFormatRegistry m_sourceFileFormatReg;
        babelwires::ProcessorFactoryRegistry m_processorReg;
        testUtils::TestDeserializationRegistry m_deserializationReg;
        babelwires::EnumRegistry m_enumReg;

        babelwires::ProjectContext m_projectContext;
        testUtils::TestLogWithListener m_log;

        babelwires::Project m_project;
    };
} // namespace testUtils
