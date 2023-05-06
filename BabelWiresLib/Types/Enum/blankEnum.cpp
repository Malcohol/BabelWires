/**
 * BlankEnum is a type just containing the "_" value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include<BabelWiresLib/Types/Enum/blankEnum.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::BlankEnum::BlankEnum()
    : EnumType({getBlankValue()}, 0)
{
}

babelwires::ShortId babelwires::BlankEnum::getBlankValue() {
    return BW_SHORT_ID("____", "____", "2c0b61a0-8f95-4957-b0bf-c6a261750c94");
}
