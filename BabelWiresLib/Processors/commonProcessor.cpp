/**
 * A convenient base class for most processors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/commonProcessor.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>

babelwires::CommonProcessor::CommonProcessor(const ProjectContext& projectContext)
    : m_inputFeature(std::make_unique<babelwires::RootFeature>(projectContext))
    , m_outputFeature(std::make_unique<babelwires::RootFeature>(projectContext)) {}

babelwires::Feature* babelwires::CommonProcessor::getInputFeature() {
    return m_inputFeature.get();
}

babelwires::Feature* babelwires::CommonProcessor::getOutputFeature() {
    return m_outputFeature.get();
}
