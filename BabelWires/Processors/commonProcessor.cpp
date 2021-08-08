/**
 * A convenient base class for most processors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Processors/commonProcessor.hpp"

babelwires::CommonProcessor::CommonProcessor()
    : m_inputFeature(std::make_unique<babelwires::RecordFeature>())
    , m_outputFeature(std::make_unique<babelwires::RecordFeature>()) {}

babelwires::RecordFeature* babelwires::CommonProcessor::getInputFeature() {
    return m_inputFeature.get();
}

babelwires::RecordFeature* babelwires::CommonProcessor::getOutputFeature() {
    return m_outputFeature.get();
}
