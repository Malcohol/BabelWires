#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <cassert>

namespace testUtils {
    /// Like TestType but it returns whatever typeRef it's told to.
    class TestConstructedType : public TestType {
      public:
        /// Takes the typeRef to return.
        TestConstructedType(babelwires::TypeRef thisTypeRef)
            : m_thisTypeRef(thisTypeRef) {}

        babelwires::TypeRef getTypeRef() const override { return m_thisTypeRef; }

        babelwires::TypeRef m_thisTypeRef;
    };

    /// Always returns TestConstructedType.
    class TestUnaryTypeConstructor : public babelwires::TypeConstructor {
      public:
        TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(getTestRegisteredLongIdentifier("Unary"), 1);

        unsigned int getArity() const override { return 1; }

        std::unique_ptr<babelwires::Type>
        constructType(babelwires::TypeRef newTypeRef,
                      const std::vector<const babelwires::Type*>& arguments) const override;
        /// A < B => UNARY<A> < UNARY<B>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& argumentsA,
                                                      const babelwires::TypeConstructorArguments& argumentsB) const;

        /// B < A => B < UNARY<A>
        babelwires::SubtypeOrder compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                      const babelwires::TypeConstructorArguments& arguments,
                                                      const babelwires::TypeRef& other) const;
    };
} // namespace testUtils
