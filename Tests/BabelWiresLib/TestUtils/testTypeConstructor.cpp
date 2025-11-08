#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>

babelwires::TypeConstructor::TypeConstructorResult testUtils::TestUnaryTypeConstructor::constructType(
    const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef,
    const babelwires::TypeConstructorArguments& arguments,
    const std::vector<const babelwires::Type*>& resolvedTypeArguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    const TestType* const sourceType = resolvedTypeArguments[0]->as<TestType>();
    if (!sourceType) {
        throw new babelwires::TypeSystemException();
    }
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef),
                                                                   sourceType->m_maximumLength + 1);
}

babelwires::TypeConstructor::TypeConstructorResult testUtils::TestBinaryTypeConstructor::constructType(
    const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef,
    const babelwires::TypeConstructorArguments& arguments,
    const std::vector<const babelwires::Type*>& resolvedTypeArguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    const TestType* const sourceType0 = resolvedTypeArguments[0]->as<TestType>();
    const TestType* const sourceType1 = resolvedTypeArguments[1]->as<TestType>();
    if (!sourceType0 || !sourceType1) {
        throw new babelwires::TypeSystemException();
    }
    return std::make_unique<babelwires::ConstructedType<TestType>>(
        std::move(newTypeRef), sourceType0->m_maximumLength + sourceType1->m_maximumLength);
}

babelwires::TypeConstructor::TypeConstructorResult testUtils::TestMixedTypeConstructor::constructType(
    const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef,
    const babelwires::TypeConstructorArguments& arguments,
    const std::vector<const babelwires::Type*>& resolvedTypeArguments) const {
    assert(arguments.getTypeArguments().size() == 1);
    assert(arguments.getValueArguments().size() == 1);

    const TestType* const testType = resolvedTypeArguments[0]->as<TestType>();
    const babelwires::StringValue* const stringValue = arguments.getValueArguments()[0]->as<babelwires::StringValue>();

    assert(testType != nullptr);
    assert(stringValue != nullptr);

    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(
        std::move(newTypeRef), testType->m_maximumLength + stringValue->get().size(),
        testType->m_defaultValue + stringValue->get());
}
