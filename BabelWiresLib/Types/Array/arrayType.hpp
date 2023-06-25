/**
 * A type describes an array value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {
    class ArrayType : public Type {
      public:
        /// An initialSize of -1 means the initial size is the minimum size.
        ArrayType(TypeRef entryType, unsigned int minimumSize, unsigned int maximumSize, int initialSize = -1);

        const TypeRef& getEntryType() const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

      private:
        TypeRef m_entryType;
        unsigned int m_initialSize;
        unsigned int m_minimumSize;
        unsigned int m_maximumSize;
    };
} // namespace babelwires
