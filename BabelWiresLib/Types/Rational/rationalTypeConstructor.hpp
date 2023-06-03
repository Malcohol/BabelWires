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
    /// Construct a new RationalType from three RationalValues: min, max and default.
    class RationalTypeConstructor : public TypeConstructor {
      public:
        // The name format is not distinct from that of int, so there can be ambiguity. That's probably
        // alright because type names are not used for anything other than display.
        TYPE_CONSTRUCTOR("Rational", "{{{0}..{1}}}", "dc2b335e-9336-471e-bc71-466bb65229d2", 1);

        std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                            const std::vector<EditableValueHolder>& valueArguments) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& argumentsA,
                                          const TypeConstructorArguments& argumentsB) const override;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments,
                                          const TypeRef& other) const override;

      private:
        /// Throws a TypeSystem exception if the arguments are not of the expect type.
        static std::tuple<Range<Rational>, Rational>
        extractValueArguments(const std::vector<EditableValueHolder>& valueArguments);
    };
} // namespace babelwires