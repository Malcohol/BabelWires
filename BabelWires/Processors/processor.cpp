/**
 * A Processor defines a processing operation from an input feature to an output feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Processors/processor.hpp"

const babelwires::RecordFeature* babelwires::Processor::getInputFeature() const {
    return const_cast<Processor*>(this)->getInputFeature();
}

const babelwires::RecordFeature* babelwires::Processor::getOutputFeature() const {
    return const_cast<Processor*>(this)->getOutputFeature();
}
