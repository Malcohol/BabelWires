/**
 * A TypePtr provides memory managed access to a type.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <memory>
namespace babelwires {
    class Type;

    using TypePtr = std::shared_ptr<const Type>;

    template <typename TYPE> using TypePtrT = std::shared_ptr<const TYPE>;

    using WeakTypePtr = std::weak_ptr<const Type>;

    template <typename TYPE, typename... ARGS> TypePtrT<TYPE> makeType(ARGS&&... args) {
        return std::make_shared<TYPE>(std::forward<ARGS>(args)...);
    }

    template<typename TYPE, typename SOURCE_PTR>
    TypePtrT<TYPE> typeAs(SOURCE_PTR&& source) {
      return std::static_pointer_cast<const TYPE>(std::forward<SOURCE_PTR>(source));
    }

} // namespace babelwires
