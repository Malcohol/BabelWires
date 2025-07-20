#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>

babelwires::TypeConstructor::TypeConstructorResult
testUtils::TestUnaryTypeConstructor::constructType(const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::EditableValueHolder>& valueArguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    const TestType* const sourceType = typeArguments[0]->as<TestType>();
    if (!sourceType) {
        throw new babelwires::TypeSystemException();
    }
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef), sourceType->m_maximumLength + 1);
}

babelwires::TypeConstructor::TypeConstructorResult
testUtils::TestBinaryTypeConstructor::constructType(const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::EditableValueHolder>& valueArguments) const {
    // Remember the typeRef, since there's no way to reconstruct it.
    const TestType* const sourceType0 = typeArguments[0]->as<TestType>();
    const TestType* const sourceType1 = typeArguments[1]->as<TestType>();
    if (!sourceType0 || !sourceType1) {
        throw new babelwires::TypeSystemException();
    }
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef), sourceType0->m_maximumLength + sourceType1->m_maximumLength);
}

babelwires::TypeConstructor::TypeConstructorResult
testUtils::TestMixedTypeConstructor::constructType(const babelwires::TypeSystem& typeSystem, babelwires::TypeRef newTypeRef, const std::vector<const babelwires::Type*>& typeArguments,
                                                    const std::vector<babelwires::EditableValueHolder>& valueArguments) const {
    assert(typeArguments.size() == 1);
    assert(valueArguments.size() == 1);
    
    const TestType *const testType = typeArguments[0]->as<TestType>();
    const babelwires::StringValue *const stringValue = valueArguments[0]->as<babelwires::StringValue>();

    assert(testType != nullptr);
    assert(stringValue != nullptr);

    // Remember the typeRef, since there's no way to reconstruct it.
    return std::make_unique<babelwires::ConstructedType<TestType>>(std::move(newTypeRef), testType->m_maximumLength + stringValue->get().size(), testType->m_defaultValue + stringValue->get());
}
