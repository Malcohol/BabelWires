/**
 * A Processor defines a processing operation from an input feature to an output feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    struct UserLogger;
    class RootFeature;

    /// Defines a processing operation from an input feature to an output feature.
    /// This should not store any state.
    // TODO: Perhaps the design could be re-done in terms of const functions.
    class Processor {
      public:
        virtual ~Processor();
        /// Set values in the output feature based on values in the input feature.
        /// Implementation of process are allowed to throw. If they do, the ProcessorElement will be marked as failed,
        /// the exception will be logged, and the output feature will be set to default.
        virtual void process(UserLogger& userLogger) = 0;
        virtual RootFeature* getInputFeature() = 0;
        virtual RootFeature* getOutputFeature() = 0;
        const RootFeature* getInputFeature() const;
        const RootFeature* getOutputFeature() const;
    };

} // namespace babelwires
