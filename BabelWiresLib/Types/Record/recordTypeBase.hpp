/**
 * RecordTypeBase is a common base of Types of RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace babelwires {

    /// RecordTypes are compound types containing a sequence of named children.
    class RecordTypeBase : public CompoundType {
      public:
        struct Field {
            ShortId m_identifier;
            TypeRef m_type;
        };

        std::string getKind() const override;
        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
    };
}
