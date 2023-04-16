/**
 * Templates which add standard functionality to features, used via CRTP (Curiously Recurring Template Pattern).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>
#include <cassert>

namespace babelwires {
    template <typename T, int MIN, int MAX> class HasStaticSizeRange : public T {
      protected:
        virtual Range<unsigned int> doGetSizeRange() const override;
    };

} // namespace babelwires

#include <BabelWiresLib/Features/featureMixins_inl.hpp>
