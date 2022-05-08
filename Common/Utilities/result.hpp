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
    class Result {
      public:
        Result() = default;
        Result(std::string reasonWhyFailed);
        Result(const char* reasonWhyFailed);

        operator bool() const {
            return m_reasonWhyFailed.empty();
        }

        std::string getReasonWhyFailed() const;
      private:
        std::string m_reasonWhyFailed;
    };
}
