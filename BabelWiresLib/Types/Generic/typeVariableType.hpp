/**
 * The Type used at a type variable that has not been instantiated.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// The Type used at a type variable that has not been instantiated.
    class TypeVariableType : public Type {
      public:
        TypeVariableType();

        std::string getFlavour() const override;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
    };
} // namespace babelwires
