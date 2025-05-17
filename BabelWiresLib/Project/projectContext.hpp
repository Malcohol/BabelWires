/**
 * The ProjectContext provides the project with access to its fundamental dependencies (e.g. registries).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/DataContext/dataContext.hpp>

#include <random>

namespace babelwires {

    class TargetFileFormatRegistry;
    class SourceFileFormatRegistry;
    class SourceFileFormatRegistry;
    class ProcessorFactoryRegistry;
    class TypeSystem;

    /// The ProjectContext provides the project with access to its fundamental dependencies (e.g. registries).
    /// This approach is an instance of dependency injection, and is used for almost all of the dependencies.
    /// Other dependencies not included here:
    /// * The debug log, because that is needed everywhere.
    /// * The user log is provided separately, because I want fine control of its use.
    /// * The IdentifierRegistry, because I judged it too painful to pass around.
    struct ProjectContext : DataContext {
        SourceFileFormatRegistry& m_sourceFileFormatReg;
        TargetFileFormatRegistry& m_targetFileFormatReg;
        ProcessorFactoryRegistry& m_processorReg;
        TypeSystem& m_typeSystem;
        std::default_random_engine m_randomEngine;
    };

} // namespace babelwires
