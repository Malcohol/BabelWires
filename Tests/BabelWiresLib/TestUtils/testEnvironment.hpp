#pragma once

#include <Tests/TestUtils/testLog.hpp>

#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BaseLib/Context/context.hpp>
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

        /// A Context subclass that registers services in its constructor,
        /// ensuring they are available before any member initialized after it.
        struct TestContext : babelwires::Context {
            TestContext(babelwires::DeserializationRegistry& deserializationReg,
                               babelwires::RandomService& randomService,
                               babelwires::SourceFileFormatRegistry& sourceFileFormatReg,
                               babelwires::TargetFileFormatRegistry& targetFileFormatReg,
                               babelwires::ProcessorFactoryRegistry& processorReg, 
                               babelwires::TypeSystem& typeSystem);
        };

        TestContext m_projectContext;
        testUtils::TestLogWithListener m_log;

        babelwires::Project m_project;
    };
} // namespace testUtils
