#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

std::unique_ptr<babelwires::Type>
testUtils::TestUnaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef,
                                                   const std::vector<const babelwires::Type*>& arguments) const {
    assert(arguments.size() == 1);
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef));
}

/// A < B => UNARY<A> < UNARY<B>
babelwires::SubtypeOrder testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(
    const babelwires::TypeSystem& typeSystem, const babelwires::TypeConstructorArguments& argumentsA,
    const babelwires::TypeConstructorArguments& argumentsB) const {
    assert(argumentsA.m_typeArguments.size() == 1);
    assert(argumentsB.m_typeArguments.size() == 1);
    return typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
}

babelwires::SubtypeOrder
testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                          const babelwires::TypeConstructorArguments& arguments,
                                                          const babelwires::TypeRef& other) const {
    assert(arguments.m_typeArguments.size() == 1);
    const babelwires::SubtypeOrder argOrder = typeSystem.compareSubtype(arguments.m_typeArguments[0], other);
    if ((argOrder == babelwires::SubtypeOrder::IsEquivalent) || (argOrder == babelwires::SubtypeOrder::IsSupertype)) {
        return babelwires::SubtypeOrder::IsSupertype;
    }
    return babelwires::SubtypeOrder::IsUnrelated;
}

std::unique_ptr<babelwires::Type>
testUtils::TestBinaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef,
                                                    const std::vector<const babelwires::Type*>& arguments) const {
    assert(arguments.size() == 2);
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef));
}

babelwires::SubtypeOrder testUtils::TestBinaryTypeConstructor::compareSubtypeHelper(
    const babelwires::TypeSystem& typeSystem, const babelwires::TypeConstructorArguments& argumentsA,
    const babelwires::TypeConstructorArguments& argumentsB) const {
    assert(argumentsA.m_typeArguments.size() == 2);
    assert(argumentsB.m_typeArguments.size() == 2);
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
