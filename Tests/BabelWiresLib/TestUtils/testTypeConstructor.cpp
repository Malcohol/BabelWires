#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

std::unique_ptr<babelwires::Type>
testUtils::TestUnaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef,
                                                   const std::array<const babelwires::Type*, 1>& arguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef));
}

/// A < B => UNARY<A> < UNARY<B>
babelwires::SubtypeOrder testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(
    const babelwires::TypeSystem& typeSystem, const babelwires::TypeConstructorArguments<1>& argumentsA,
    const babelwires::TypeConstructorArguments<1>& argumentsB) const {
    return typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
}

babelwires::SubtypeOrder
testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                          const babelwires::TypeConstructorArguments<1>& arguments,
                                                          const babelwires::TypeRef& other) const {
    const babelwires::SubtypeOrder argOrder = typeSystem.compareSubtype(arguments.m_typeArguments[0], other);
    if ((argOrder == babelwires::SubtypeOrder::IsEquivalent) || (argOrder == babelwires::SubtypeOrder::IsSupertype)) {
        return babelwires::SubtypeOrder::IsSupertype;
    }
    return babelwires::SubtypeOrder::IsUnrelated;
}

std::unique_ptr<babelwires::Type>
testUtils::TestBinaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef,
                                                    const std::array<const babelwires::Type*, 2>& arguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef));
}

babelwires::SubtypeOrder testUtils::TestBinaryTypeConstructor::compareSubtypeHelper(
    const babelwires::TypeSystem& typeSystem, const babelwires::TypeConstructorArguments<2>& argumentsA,
    const babelwires::TypeConstructorArguments<2>& argumentsB) const {
    const auto compare0 = typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
    const auto compare1 = typeSystem.compareSubtype(argumentsA.m_typeArguments[1], argumentsB.m_typeArguments[1]);
    if ((compare0 == babelwires::SubtypeOrder::IsEquivalent) && (compare1 == babelwires::SubtypeOrder::IsEquivalent)) {
        return babelwires::SubtypeOrder::IsEquivalent;
    }
    if (((compare0 == babelwires::SubtypeOrder::IsSupertype) || (compare0 == babelwires::SubtypeOrder::IsEquivalent))
     && ((compare1 == babelwires::SubtypeOrder::IsSubtype)) || (compare1 == babelwires::SubtypeOrder::IsEquivalent)) {
        return babelwires::SubtypeOrder::IsSubtype;
    }
    if (((compare0 == babelwires::SubtypeOrder::IsSubtype) || (compare0 == babelwires::SubtypeOrder::IsEquivalent))
    && ((compare1 == babelwires::SubtypeOrder::IsSupertype) || (compare1 == babelwires::SubtypeOrder::IsEquivalent))) {
        return babelwires::SubtypeOrder::IsSupertype;
    }
    return babelwires::SubtypeOrder::IsUnrelated;
}
