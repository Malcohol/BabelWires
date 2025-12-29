/**
 * A TypePtr provides memory managed access to a type.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// A pointer to a memory managed type.
    using TypePtr = std::shared_ptr<const Type>;

    /// Construct a type in the correct way.
    template<typename TYPE, typename... ARGS>
    TypePtr makeType(ARGS&&... args) {
      return std::make_shared<TYPE>(std::forward<ARGS>(args)...);
    }

    /// A pointer to a specific-class of memory managed type.
    template<typename TYPE = Type>
    using TypePtrT = std::shared_ptr<const TYPE>;

} // namespace babelwires
