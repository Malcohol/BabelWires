/**
 * A StringFeature is a ValueFeature which carries a string.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/features.hpp"

#include <string>

namespace babelwires {

    class StringFeature : public ValueFeature {
      public:
        std::string get() const;
        void set(std::string value);

      protected:
        virtual void doSetToDefault() override;
        virtual std::string doGetValueType() const override;
        virtual void doAssign(const ValueFeature& other) override;
        virtual std::size_t doGetHash() const override;

      private:
        std::string m_value;
    };

} // namespace babelwires
