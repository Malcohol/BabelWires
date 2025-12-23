/**
 * The FailureType has this as its only value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/editableValue.hpp>

namespace babelwires {

    /// The FailureType has this as its only value.
    /// It carried no data, but FailureTypes can be asked to create a value so there needs to
    /// be at least one value.
    class FailureValue : public Value {
      public:
        CLONEABLE(FailureValue);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
    };

} // namespace babelwires
