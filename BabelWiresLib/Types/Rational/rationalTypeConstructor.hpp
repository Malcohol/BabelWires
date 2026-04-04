/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <BaseLib/Math/rational.hpp>

namespace babelwires {
    /// Construct a new RationalType from three RationalValues: min, max and default.
    class BABELWIRESLIB_API RationalTypeConstructor : public TypeConstructor {
      public:
        DOWNCASTABLE(RationalTypeConstructor, TypeConstructor);
        // The name format is not distinct from that of int, so there can be ambiguity. That's probably
        // alright because type names are not used for anything other than display.
        TYPE_CONSTRUCTOR("Rational", "Rational{{[0]..[1]}}", "dc2b335e-9336-471e-bc71-466bb65229d2", 1);

        ResultT<TypePtr> constructType(const TypeSystem& typeSystem, TypeExp newTypeExp, const TypeConstructorArguments& arguments,
                                            const std::vector<TypePtr>& resolvedTypeArguments) const override;

        /// Convenience method.
        static TypeExp makeTypeExp(Rational min, Rational max, Rational defaultValue = 0);

      private:
        /// Returns an error if the arguments are not of the expect type.
        static ResultT<std::tuple<Range<Rational>, Rational>>
        extractValueArguments(const std::vector<ValueHolder>& valueArguments);
    };
} // namespace babelwires