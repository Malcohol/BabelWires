/**
 * The ProcessorFactoryRegistry is a registry for ProcessorFactories.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Registry/registry.hpp>
#include <Common/exceptions.hpp>

namespace babelwires {

    class ProcessorFactory;

    /// A registry for processor factories.
    class ProcessorFactoryRegistry : public Registry<ProcessorFactory> {
      public:
        ProcessorFactoryRegistry();

        template<typename PROCESSOR_SUBTYPE>
        ProcessorFactory* addProcessor() {
          return addEntry<typename PROCESSOR_SUBTYPE::template ThisProcessorFactory<PROCESSOR_SUBTYPE> >();
        }
    };

} // namespace babelwires
