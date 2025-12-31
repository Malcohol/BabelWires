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

    template <typename TYPE, typename... ARGS> TypePtr makeType(ARGS&&... args) {
        return std::make_shared<TYPE>(std::forward<ARGS>(args)...);
    }

    template<typename TYPE, typename SOURCE_PTR>
    TypePtrT<TYPE> typeAs(SOURCE_PTR&& source) {
      return std::static_pointer_cast<const TYPE>(std::forward<SOURCE_PTR>(source));
    }


    /*
    template<typename TYPE> class TypePtrT;

    class TypePtr final {
      public:
        TypePtr() = default;
        //~TypePtr();

        const Type* operator->() const {
          return m_type.get();
        }
        const Type* get() const {
          return m_type.get();
        }
        const Type& operator*() const {
          return *m_type;
        }
        operator bool() const {
          return m_type.get();
        }

        template <typename TYPE, typename... ARGS> static TypePtr makeType(ARGS&&... args) {
            return TypePtr(std::make_shared<TYPE>(std::forward<ARGS>(args)...));
        }

      private:
        TypePtr(std::shared_ptr<const Type> type);

      private:
        template<typename TYPE> friend class TypePtrT;

        std::shared_ptr<const Type> m_type;
    };

    template<typename TYPE>
    class TypePtrT final {
      public:
        TypePtrT() = default;
        TypePtrT(TypePtr type)
          : m_type(std::static_pointer_cast<const TYPE>(type.m_type)) {
            assert(m_type->template as<TYPE>());
          }
        template<typename U>
        TypePtrT(const TypePtrT<U>&& type)
          : m_type(std::static_pointer_cast<const TYPE>(type.m_type)) {
            assert(m_type->template as<TYPE>());
          }

        const TYPE* operator->() const {
          return m_type.get();
        }
        const TYPE* get() const {
          return m_type.get();
        }
        const TYPE& operator*() const {
          return *m_type;
        }
        operator bool() const {
          return m_type.get();
        }

      private:
        template<typename U> friend class TypePtrT;

        std::shared_ptr<const TYPE> m_type;
    };
    */

} // namespace babelwires
