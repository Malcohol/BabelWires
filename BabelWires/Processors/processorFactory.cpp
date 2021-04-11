/**
 * The ProcessorFactory is an abstraction for factories which create processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Processors/processorFactory.hpp"

babelwires::ProcessorFactory::ProcessorFactory(std::string identifier, std::string name, VersionNumber version)
    : RegistryEntry(std::move(identifier), std::move(name), version) {}
