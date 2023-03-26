/**
 * The type of strings.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    class StringType : public Type {
      public:
        PRIMITIVE_TYPE("string", "string", "0e422e25-cb94-40a3-8790-4918c918e637", 1);

        std::unique_ptr<Value> createValue() const override;
    };
}
