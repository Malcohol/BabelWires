/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Common/Math/rational.hpp>

namespace babelwires {
    class RationalTypeConstructor : public TypeConstructor {
      public:
        // TODO Need distinct name from Int.
        TYPE_CONSTRUCTOR("Rational", "{{{0}..{2}..{1}}}", "dc2b335e-9336-471e-bc71-466bb65229d2", 1);

        std::unique_ptr<Type> constructType(TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<ValueHolder>& valueArguments) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& argumentsA,
                                          const TypeConstructorArguments& argumentsB) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments,
                                          const TypeRef& other) const override;

      private:
        /// Throws a TypeSystem exception if the arguments are not of the expect type.
        static std::tuple<Range<Rational>, Rational>
        extractValueArguments(const std::vector<ValueHolder>& valueArguments);
    };
} // namespace babelwires