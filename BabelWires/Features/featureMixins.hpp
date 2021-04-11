/**
 * Templates which add standard functionality to features, used via CRTP (Curiously Recurring Template Pattern).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/features.hpp"
#include <cassert>

namespace babelwires {

    template <typename T, int LOW, int HIGH> class HasStaticRange : public T {
      public:
        virtual Range<typename T::ValueType> getRange() const override;
    };

    template <typename T, int DEFAULT> class HasStaticDefault : public T {
      public:
        virtual void doSetToDefault() override;
    };

    template <typename T, int MIN, int MAX> class HasStaticSizeRange : public T {
      protected:
        virtual Range<unsigned int> doGetSizeRange() const override;
    };

} // namespace babelwires

#include "BabelWires/Features/featureMixins_inl.hpp"
