/**
 * A StringFeature is a ValueFeature which carries a string.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <string>

namespace babelwires {

    class StringFeature : public SimpleValueFeature {
      public:
        StringFeature();
        std::string get() const;
        void set(std::string value);
    };

} // namespace babelwires
