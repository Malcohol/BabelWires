/**
 * BlankEnum is a type just containing the "_" value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {
    
    /// BlankEnum is a type just containing the "_" value.
    class BlankEnum : public babelwires::EnumType {
      public:
        PRIMITIVE_TYPE("BlankEnum", "_", "632eca3b-1fbe-4de8-aade-f914add1526e", 1);
        BlankEnum();

        /// Note that the blank value is only permitted at the end of the enum.
        static ShortId getBlankValue();
    };
}
