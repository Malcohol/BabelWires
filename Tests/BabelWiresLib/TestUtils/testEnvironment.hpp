#pragma once

#include <Tests/TestUtils/testLog.hpp>

#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BaseLib/Random/randomService.hpp>
#include <BaseLib/Serialization/deserializationRegistry.hpp>

namespace testUtils {
    struct TestEnvironment {
        TestEnvironment();

        babelwires::TargetFileFormatRegistry m_targetFileFormatReg;
        babelwires::SourceFileFormatRegistry m_sourceFileFormatReg;
        babelwires::ProcessorFactoryRegistry m_processorReg;
        babelwires::DeserializationRegistry m_deserializationReg;
        babelwires::RandomService m_randomService;
        babelwires::TypeSystem m_typeSystem;

        babelwires::ProjectContext m_projectContext;
        testUtils::TestLogWithListener m_log;

        babelwires::Project m_project;
    };
} // namespace testUtils
