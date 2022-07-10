/**
 * A result class which can carry an explanation why it failed.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>

namespace babelwires {
    /// A simple result class which carries a reason why it failed if it isn't success.
    class Result {
      public:
        /// Allow unambiguous construction in the successful case.
        enum Success { success };

        /// Construct a successful result.
        Result(Success) {}
        
        /// Construct a failed result with a justification.
        Result(std::string reasonWhyFailed);
        
        /// Construct a failed result with a justification.
        Result(const char* reasonWhyFailed);

        /// Return true if successful.
        operator bool() const {
            return m_reasonWhyFailed.empty();
        }

        /// Get the reason why the result was not successful.
        std::string getReasonWhyFailed() const;

      private:
        /// Empty means successful.
        std::string m_reasonWhyFailed;
    };
}
