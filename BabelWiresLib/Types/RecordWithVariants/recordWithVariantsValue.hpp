/**
 * RecordWithVariantsType is like a RecordType but has a number of variants.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordValue.hpp>

#include <vector>

namespace babelwires {
    /// RecordValues are compound values containing a set of named children.
    class RecordWithVariantsValue : public RecordValue {
      public:
        CLONEABLE(RecordWithVariantsValue);

        RecordWithVariantsValue(ShortId tag);

        void setTag(ShortId tag);
        ShortId getTag() const;

      private:
        ShortId m_tag;
    };

} // namespace babelwires
