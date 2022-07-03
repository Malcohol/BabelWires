/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    class IntType : public Type {
      public:
        IntType();

        static LongIdentifier getThisIdentifier();

        std::unique_ptr<Value> createValue() const override;
    };

}
