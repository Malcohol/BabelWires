/**
 * ListType is the type for ListValue.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// A list type which contains elements of a single type.
    class ListType : public Type {
      public:
        ListType(TypeRef elementTypeRef);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getFlavour() const override;

        const TypeRef& getElementTypeRef() const;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem,
                                                         const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      private:
        TypeRef m_elementTypeRef;
    };
} // namespace babelwires