/**
 * The ProcessorFactory is an abstraction for factories which create processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Registry/registry.hpp>

#include <memory>
#include <string>

namespace babelwires {

    class Processor;
    struct ProjectContext;

    /// Objects which can create processors, and which can be registered in the ProcessFactoryRegistry.
    class ProcessorFactory : public RegistryEntry {
      public:
        ProcessorFactory(LongId identifier, VersionNumber version);

        /// Return a new processor.
        virtual std::unique_ptr<Processor> createNewProcessor(const ProjectContext& projectContext) const = 0;
    };

} // namespace babelwires
