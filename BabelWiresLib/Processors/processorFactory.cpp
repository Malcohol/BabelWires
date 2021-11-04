/**
 * The ProcessorFactory is an abstraction for factories which create processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Processors/processorFactory.hpp"

babelwires::ProcessorFactory::ProcessorFactory(LongIdentifier identifier, VersionNumber version)
    : RegistryEntry(identifier, version) {}
