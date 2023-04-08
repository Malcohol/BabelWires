/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

namespace babelwires {
    class IntTypeConstructor : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("Int", "{{{0}..{2}..{1}}}", "96dc61c3-5940-47c4-9d98-9f06d5f01157", 1);

        std::unique_ptr<Type> constructType(TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<ValueHolder>& valueArguments) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& argumentsA,
                                          const TypeConstructorArguments& argumentsB) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments,
                                          const TypeRef& other) const override;
    };
} // namespace babelwires