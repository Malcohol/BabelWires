#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <cassert>

namespace testUtils {
    /// Always returns a TestType.
    class TestUnaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredLongIdentifier("Unary"), 1);

        unsigned int getArity() const override { return 1; }

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef,
                      const std::vector<const babelwires::Type*>& arguments) const override;
        /// A < B => Unary<A> < Unary<B>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& argumentsA,
                                                      const babelwires::TypeConstructorArguments& argumentsB) const;

        /// Same as AddBlank.
        /// B < A => B < Unary<A>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& arguments,
                                                      const babelwires::TypeRef& other) const;
    };

    class TestBinaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredLongIdentifier("Binary"), 1);
        unsigned int getArity() const override { return 2; }

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef,
                      const std::vector<const babelwires::Type*>& arguments) const override;

        /// Similar to a function type constructor.
        /// (B < A) && (C < D) => Binary<A, C> < Binary<B, D>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& argumentsA,
                                                      const babelwires::TypeConstructorArguments& argumentsB) const;
    };

} // namespace testUtils
