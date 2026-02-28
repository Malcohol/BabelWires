#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BaseLib/Result/error.hpp>

babelwires::ResultT<babelwires::TypePtr> testUtils::TestUnaryTypeConstructor::constructType(
    const babelwires::TypeSystem& typeSystem, babelwires::TypeExp newTypeExp,
    const babelwires::TypeConstructorArguments& arguments,
    const std::vector<babelwires::TypePtr>& resolvedTypeArguments) const {
    // Remember the typeExp, since there's no way to reconstruct it.
    const TestType* const sourceType = resolvedTypeArguments[0]->tryAs<TestType>();
    if (!sourceType) {
        return babelwires::Error();
    }
    return babelwires::makeType<TestType>(std::move(newTypeExp), sourceType->m_maximumLength + 1);
}

babelwires::ResultT<babelwires::TypePtr> testUtils::TestBinaryTypeConstructor::constructType(
    const babelwires::TypeSystem& typeSystem, babelwires::TypeExp newTypeExp,
    const babelwires::TypeConstructorArguments& arguments,
    const std::vector<babelwires::TypePtr>& resolvedTypeArguments) const {
    // Remember the typeExp, since there's no way to reconstruct it.
    const TestType* const sourceType0 = resolvedTypeArguments[0]->tryAs<TestType>();
    const TestType* const sourceType1 = resolvedTypeArguments[1]->tryAs<TestType>();
    if (!sourceType0 || !sourceType1) {
        return babelwires::Error();
    }
    return babelwires::makeType<TestType>(std::move(newTypeExp),
                                          sourceType0->m_maximumLength + sourceType1->m_maximumLength);
}

babelwires::ResultT<babelwires::TypePtr> testUtils::TestMixedTypeConstructor::constructType(
    const babelwires::TypeSystem& typeSystem, babelwires::TypeExp newTypeExp,
    const babelwires::TypeConstructorArguments& arguments,
    const std::vector<babelwires::TypePtr>& resolvedTypeArguments) const {
    assert(arguments.getTypeArguments().size() == 1);
    assert(arguments.getValueArguments().size() == 1);

    const TestType* const testType = resolvedTypeArguments[0]->tryAs<TestType>();
    const babelwires::StringValue* const stringValue = arguments.getValueArguments()[0]->tryAs<babelwires::StringValue>();

    assert(testType != nullptr);
    assert(stringValue != nullptr);

    // Remember the typeExp, since there's no way to reconstruct it.
    return babelwires::makeType<TestType>(std::move(newTypeExp), testType->m_maximumLength + stringValue->get().size(),
                                          testType->m_defaultValue + stringValue->get());
}
