/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    /// Construct a new IntType from three IntValues: min, max and default.
    class IntTypeConstructor : public TypeConstructor {
      public:
        /// Note that the we don't represent the default in the name.
        TYPE_CONSTRUCTOR("Int", "Integer{{[0]..[1]}}", "96dc61c3-5940-47c4-9d98-9f06d5f01157", 1);

        TypeConstructorResult constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        /// Convenience method.
        static TypeRef makeTypeRef(IntValue::NativeType min, IntValue::NativeType max, IntValue::NativeType defaultValue = 0);
      private:
        /// Throws a TypeSystem exception if the arguments are not of the expected type.
        static std::tuple<Range<IntValue::NativeType>, IntValue::NativeType>
        extractValueArguments(const std::vector<EditableValueHolder>& valueArguments);
    };
} // namespace babelwires