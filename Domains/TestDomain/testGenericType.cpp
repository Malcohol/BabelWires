#include <Domains/TestDomain/testGenericType.hpp>

#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>

testDomain::TestGenericType::TestGenericType()
: GenericType(
    // Note: We assume generic types are made using TypeRef expressions, so this cannot
    // use a registered record type. Similarly, the array cannot be a registered type.
    babelwires::RecordTypeConstructor::makeTypeRef(
        BW_SHORT_ID("x", "x", "2bff1eef-8ca5-4988-85f7-7fb1f29e629c"), 
        babelwires::TypeVariableTypeConstructor::makeTypeRef(),
        BW_SHORT_ID("int", "int", "ffa6f718-4b8c-4797-8037-52216be06cde"), 
        babelwires::DefaultIntType::getThisType(),
        BW_SHORT_ID("array", "arr", "2c4d9dfc-100c-4a56-9323-ff8f420b91ce"), 
        babelwires::ArrayTypeConstructor::makeTypeRef(
            babelwires::TypeVariableTypeConstructor::makeTypeRef(),
            0, 8
        )
    ),
    1
)
{
}