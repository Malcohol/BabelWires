/**
 * A Processor defines a processing operation from an input feature to an output feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/processor.hpp>

const babelwires::RootFeature* babelwires::Processor::getInputFeature() const {
    return const_cast<Processor*>(this)->getInputFeature();
}

const babelwires::RootFeature* babelwires::Processor::getOutputFeature() const {
    return const_cast<Processor*>(this)->getOutputFeature();
}
