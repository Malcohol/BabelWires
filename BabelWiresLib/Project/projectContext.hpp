/**
 * The ProjectContext provides the project with access to its fundimental dependencies (e.g. registries).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <random>
namespace babelwires {
    class DeserializationRegistry;
}

namespace babelwires {

    class TargetFileFormatRegistry;
    class SourceFileFormatRegistry;
    class ProcessorFactoryRegistry;
    class EnumRegistry;

    /// The ProjectContext provides the project with access to its fundimental dependencies (e.g. registries).
    /// This approach is an instance of dependency injection, and is used for almost all of the dependencies.
    /// Other dependencies not included here:
    /// * The user log is provided separately, because I want finer control of its use. (TODO Review this.)
    /// * The debug log, because that is needed everywhere.
    /// * The FieldNameRegistry, because I judged it too painful to pass around.
    struct ProjectContext {
        SourceFileFormatRegistry& m_sourceFileFormatReg;
        TargetFileFormatRegistry& m_targetFileFormatReg;
        ProcessorFactoryRegistry& m_processorReg;
        DeserializationRegistry& m_deserializationReg;
        EnumRegistry& m_enumRegistry;
        std::default_random_engine m_randomEngine;
    };

} // namespace babelwires
