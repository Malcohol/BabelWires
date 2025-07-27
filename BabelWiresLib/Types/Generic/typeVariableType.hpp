/**
 * The Type used at a type variable that has not been instantiated.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

namespace babelwires {

    /// The Type used at a type variable that has not been instantiated.
    /// The information about the type variable is carried by the typeRef.
    class TypeVariableType : public Type {
      public:
        TypeVariableType();

        std::string getFlavour() const override;

        /// Convenience method that extracts the data from this type's TypeRef.
        /// This method asserts if this type was not constructed in the expected way.
        TypeVariableTypeConstructor::VariableData getVariableData() const;

        static constexpr unsigned int c_maxNumTypeVariables = 26;

        static std::string toString(TypeVariableTypeConstructor::VariableData variableData);

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
    };
} // namespace babelwires
