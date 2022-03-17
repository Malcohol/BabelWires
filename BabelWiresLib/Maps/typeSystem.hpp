/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class EnumRegistry;

    class TypeSystem {
      public:
        TypeSystem(const EnumRegistry& enumRegistry);

        /// Built-in types.
        enum class Kind { Int, Enum, NotAKind };

        /// Asserts that the type is a builtIn.
        static LongIdentifier getBuiltInTypeId(Kind type);

        Kind getTypeFromIdentifier(LongIdentifier id) const;

        bool isAValue(Identifier id, LongIdentifier type) const;
        bool isAValue(int value, LongIdentifier type) const;

      private:
        const EnumRegistry& m_enumRegistry;
    };

} // namespace babelwires
