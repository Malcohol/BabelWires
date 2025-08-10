#include <Domains/TestDomain/testGenericType.hpp>

#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>

testDomain::TestGenericType::TestGenericType()
: GenericType(
    // Note: We assume generic types are made using TypeRef expressions, so this cannot
    // use a registered record type. Similarly, the array cannot be a registered type.
    babelwires::RecordTypeConstructor::makeTypeRef(
        BW_SHORT_ID("x", "x", "2bff1eef-8ca5-4988-85f7-7fb1f29e629c"), 
        babelwires::TypeVariableTypeConstructor::makeTypeRef(),
        BW_SHORT_ID("y", "y", "ff0b9780-e3f3-46b6-a7ae-ac961425f9e8"), 
        babelwires::TypeVariableTypeConstructor::makeTypeRef(1),
        BW_SHORT_ID("int", "int", "ffa6f718-4b8c-4797-8037-52216be06cde"), 
        babelwires::DefaultIntType::getThisType(),
        BW_SHORT_ID("nesgen", "nested generic", "c80d399e-25c3-4f24-96a4-ce908789ccdf"), 
        babelwires::GenericTypeConstructor::makeTypeRef(
            babelwires::RecordTypeConstructor::makeTypeRef(
                BW_SHORT_ID("x", "x", "df7c5f8c-8544-40b8-9123-c050d1c2ec8b"), 
                babelwires::TypeVariableTypeConstructor::makeTypeRef(0, 1),
                BW_SHORT_ID("z", "z", "e455ef6c-9bf4-4da7-8308-d45501f7d43d"), 
                babelwires::TypeVariableTypeConstructor::makeTypeRef()
            )
        ),
        BW_SHORT_ID("array", "arr", "2c4d9dfc-100c-4a56-9323-ff8f420b91ce"), 
        babelwires::ArrayTypeConstructor::makeTypeRef(
            babelwires::TypeVariableTypeConstructor::makeTypeRef(),
            0, 8
        )
    ),
    2)
{
}