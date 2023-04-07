#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <cassert>

namespace testUtils {
    /// Always returns a TestType.
    class TestUnaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Unary"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::ValueHolder>& valueArguments) const override;

        /// A < B => Unary<A> < Unary<B>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& argumentsA,
                                                      const babelwires::TypeConstructorArguments& argumentsB) const;

        /// Same as AddBlankToEnum.
        /// B < A => B < Unary<A>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& arguments,
                                                      const babelwires::TypeRef& other) const;
    };

    class TestBinaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Binary"), 1);

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::ValueHolder>& valueArguments) const override;

        /// Similar to a function type constructor.
        /// (B < A) && (C < D) => Binary<A, C> < Binary<B, D>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& argumentsA,
                                                      const babelwires::TypeConstructorArguments& argumentsB) const;
    };

} // namespace testUtils
