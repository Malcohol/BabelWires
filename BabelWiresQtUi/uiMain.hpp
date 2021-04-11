/**
 * Framework-agnostic entry point for creating and running the UI.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <memory>

namespace babelwires {
    class UnifiedLog;
    class UiProjectContext;

    /// Manages the creation of the UI.
    class Ui {
      public:
        /// A UI framework may consume arguments from the command-line.
        Ui(int& argc, char** argv, UiProjectContext& projectContext, UnifiedLog& log);

        virtual ~Ui();

        /// Run the main loop and return when the application is closed.
        void runMainLoop();

      private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace babelwires
