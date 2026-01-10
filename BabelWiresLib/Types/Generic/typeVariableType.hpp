/**
 * The Type used at a type variable that has not been instantiated.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>

namespace babelwires {

    /// The Type used at a type variable that has not been instantiated.
    /// The information about the type variable is carried by the typeExp.
    class TypeVariableType : public Type {
      public:
        TypeVariableType(TypeExp&& typeExpOfThis);

        std::string getFlavour() const override;

        /// Convenience method that extracts the data from this type's TypeExp.
        /// The method asserts if this type was not constructed in the expected way.
        TypeVariableData getVariableData() const;

        /// A guarantee that this type has no children.
        constexpr static unsigned int c_numChildren = 0;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
    };
} // namespace babelwires
