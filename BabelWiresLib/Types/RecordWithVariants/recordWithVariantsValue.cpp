/**
 * RecordWithVariantsType is like a RecordType but has a number of variants.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>

babelwires::RecordWithVariantsValue::RecordWithVariantsValue(ShortId tag) : m_tag(tag) {}

void babelwires::RecordWithVariantsValue::setTag(ShortId tag) { m_tag = tag; }
babelwires::ShortId babelwires::RecordWithVariantsValue::getTag() const { return m_tag; }
