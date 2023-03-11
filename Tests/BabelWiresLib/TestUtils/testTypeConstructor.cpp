#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

std::unique_ptr<babelwires::Type>
testUtils::TestUnaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef,
                const std::vector<const babelwires::Type*>& arguments) const {
    assert(arguments.size() == 1);
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<TestConstructedType>(std::move(newTypeRef));
}

/// A < B => UNARY<A> < UNARY<B>
babelwires::SubtypeOrder testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                const babelwires::TypeConstructorArguments& argumentsA,
                                                const babelwires::TypeConstructorArguments& argumentsB) const {
    assert(argumentsA.m_typeArguments.size() == 1);
    assert(argumentsB.m_typeArguments.size() == 1);
    return typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
}

/// B < A => B < UNARY<A>
babelwires::SubtypeOrder testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                const babelwires::TypeConstructorArguments& arguments,
                                                const babelwires::TypeRef& other) const {
    if (arguments.m_typeArguments.size() != 1) {
        return babelwires::SubtypeOrder::IsUnrelated;
    }
    const babelwires::SubtypeOrder argOrder = typeSystem.compareSubtype(arguments.m_typeArguments[0], other);
    if ((argOrder == babelwires::SubtypeOrder::IsEquivalent) || (argOrder == babelwires::SubtypeOrder::IsSupertype)) {
        return babelwires::SubtypeOrder::IsSupertype;
    }
    return babelwires::SubtypeOrder::IsUnrelated;
}
