/**
 * The ProcessorFactoryRegistry is a registry for ProcessorFactories.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>

babelwires::ProcessorFactoryRegistry::ProcessorFactoryRegistry()
    : Registry<ProcessorFactory>("Processor Factory Registry") {}
