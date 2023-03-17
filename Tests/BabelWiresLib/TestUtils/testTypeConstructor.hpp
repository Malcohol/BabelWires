#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <cassert>

namespace testUtils {
    /// Always returns a TestType.
    class TestUnaryTypeConstructor : public babelwires::TypeConstructor<1> {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Unary"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef,
                      const std::array<const babelwires::Type*, 1>& arguments) const override;

        /// A < B => Unary<A> < Unary<B>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments<1>& argumentsA,
                                                      const babelwires::TypeConstructorArguments<1>& argumentsB) const;

        /// Same as AddBlankToEnum.
        /// B < A => B < Unary<A>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments<1>& arguments,
                                                      const babelwires::TypeRef& other) const;
    };

    class TestBinaryTypeConstructor : public babelwires::TypeConstructor<2> {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Binary"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef,
                      const std::array<const babelwires::Type*, 2>& arguments) const override;

        /// Similar to a function type constructor.
        /// (B < A) && (C < D) => Binary<A, C> < Binary<B, D>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments<2>& argumentsA,
                                                      const babelwires::TypeConstructorArguments<2>& argumentsB) const;
    };

} // namespace testUtils
