/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    class IntType : public Type {
      public:
        PRIMITIVE_TYPE("int", "integer", "90ed4c0c-2fa1-4373-9b67-e711358af824", 1);

        std::unique_ptr<Value> createValue() const override;

        bool isValidValue(const Value& v) const override;
    };
}
