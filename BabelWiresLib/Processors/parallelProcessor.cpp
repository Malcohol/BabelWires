/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>

#include <algorithm>
#include <numeric>
#include <array>

void babelwires::parallelProcessorHelper(
    const RecordFeature* inputFeature, const babelwires::ArrayFeature* arrayIn, babelwires::ArrayFeature* arrayOut,
    babelwires::UserLogger& userLogger,
    std::function<void(babelwires::UserLogger&, const Feature*, Feature*)> processEntry) {
    if (arrayIn->isChanged(babelwires::Feature::Changes::StructureChanged)) {
        // Keep the out structure in sync with the in structure, and
        // try to ensure that tracks which correspond before still
        // correspond afterwards.
        arrayOut->copyStructureFrom(*arrayIn);
    }

    bool nonArrayInputChange = false;
    for (auto i : subfeatures(inputFeature)) {
        if ((i != arrayIn) && i->isChanged(Feature::Changes::SomethingChanged)) {
            nonArrayInputChange = true;
            break;
        }
    }

    std::array<int, s_maxParallelFeatures> indices;
    bool isFailed = false;
    std::array<std::string, s_maxParallelFeatures> failureStrings;
    std::iota(indices.begin(), indices.end(), 0);
    // TODO Parallel execution.
    std::for_each(indices.begin(), indices.begin() + arrayIn->getNumFeatures(),
                  [&arrayIn, &arrayOut, &userLogger, &failureStrings, &isFailed, &processEntry, nonArrayInputChange](int i) {
                      const Feature* inputEntry = arrayIn->getFeature(i);
                      if (nonArrayInputChange || inputEntry->isChanged(Feature::Changes::SomethingChanged)) {
                        try {
                            processEntry(userLogger, inputEntry, arrayOut->getFeature(i));
                        } catch (const BaseException& e) {
                            failureStrings[i] = e.what();
                            isFailed = true;
                        }
                      }
                  });
    if (isFailed) {
        // TODO Throw a compound exception.
    }
}
