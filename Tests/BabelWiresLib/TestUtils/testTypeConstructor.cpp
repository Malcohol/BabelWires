#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>

std::unique_ptr<babelwires::Type>
testUtils::TestUnaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::ValueHolder>& valueArguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef));
}

/// A < B => UNARY<A> < UNARY<B>
babelwires::SubtypeOrder testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(
    const babelwires::TypeSystem& typeSystem, const babelwires::TypeConstructorArguments& argumentsA,
    const babelwires::TypeConstructorArguments& argumentsB) const {
    return typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
}

babelwires::SubtypeOrder
testUtils::TestUnaryTypeConstructor::compareSubtypeHelper(const babelwires::TypeSystem& typeSystem,
                                                          const babelwires::TypeConstructorArguments& arguments,
                                                          const babelwires::TypeRef& other) const {
    const babelwires::SubtypeOrder argOrder = typeSystem.compareSubtype(arguments.m_typeArguments[0], other);
    if ((argOrder == babelwires::SubtypeOrder::IsEquivalent) || (argOrder == babelwires::SubtypeOrder::IsSupertype)) {
        return babelwires::SubtypeOrder::IsSupertype;
    }
    return babelwires::SubtypeOrder::IsUnrelated;
}

std::unique_ptr<babelwires::Type>
testUtils::TestBinaryTypeConstructor::constructType(babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::ValueHolder>& valueArguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef));
}

babelwires::SubtypeOrder testUtils::TestBinaryTypeConstructor::compareSubtypeHelper(
    const babelwires::TypeSystem& typeSystem, const babelwires::TypeConstructorArguments& argumentsA,
    const babelwires::TypeConstructorArguments& argumentsB) const {
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

std::unique_ptr<babelwires::Type>
testUtils::TestMixedTypeConstructor::constructType(babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::ValueHolder>& valueArguments) const {
    assert(typeArguments.size() == 1);
    assert(valueArguments.size() == 1);
    
    const TestType *const testType = typeArguments[0]->as<TestType>();
    const babelwires::StringValue *const stringValue = valueArguments[0]->as<babelwires::StringValue>();

    assert(testType != nullptr);
    assert(stringValue != nullptr);

    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef), testType->m_defaultValue + stringValue->get());
}
