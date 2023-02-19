#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(TypeRefTest, equality) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef primitiveTypeRef1(babelwires::LongIdentifier("Foo"));
    babelwires::TypeRef primitiveTypeRef2(babelwires::LongIdentifier("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::LongIdentifier("Foo"), {babelwires::LongIdentifier("Bar")});
    babelwires::TypeRef constructedTypeRef2(babelwires::LongIdentifier("Foo"),
                                            {babelwires::LongIdentifier("Bar"), babelwires::LongIdentifier("Flerm")});
    babelwires::TypeRef constructedTypeRef3(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"),
         babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")})});

    EXPECT_EQ(nullTypeRef, nullTypeRef);
    EXPECT_EQ(primitiveTypeRef1, primitiveTypeRef1);
    EXPECT_EQ(primitiveTypeRef2, primitiveTypeRef2);
    EXPECT_EQ(constructedTypeRef1, constructedTypeRef1);
    EXPECT_EQ(constructedTypeRef2, constructedTypeRef2);
    EXPECT_EQ(constructedTypeRef3, constructedTypeRef3);

    EXPECT_NE(nullTypeRef, primitiveTypeRef1);
    EXPECT_NE(nullTypeRef, primitiveTypeRef2);
    EXPECT_NE(nullTypeRef, constructedTypeRef1);
    EXPECT_NE(nullTypeRef, constructedTypeRef2);
    EXPECT_NE(nullTypeRef, constructedTypeRef3);

    EXPECT_NE(primitiveTypeRef1, primitiveTypeRef2);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRef1);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRef2);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRef3);

    EXPECT_NE(primitiveTypeRef2, constructedTypeRef1);
    EXPECT_NE(primitiveTypeRef2, constructedTypeRef2);
    EXPECT_NE(primitiveTypeRef2, constructedTypeRef3);

    EXPECT_NE(constructedTypeRef1, constructedTypeRef2);
    EXPECT_NE(constructedTypeRef1, constructedTypeRef3);

    EXPECT_NE(constructedTypeRef2, constructedTypeRef3);
}

TEST(TypeRefTest, serializeToStringNoDiscriminators) {
    babelwires::TypeRef nullTypeRef;
    EXPECT_EQ(nullTypeRef.serializeToString(), "<>");

    babelwires::TypeRef primitiveTypeRef(babelwires::LongIdentifier("Foo"));
    EXPECT_EQ(primitiveTypeRef.serializeToString(), "Foo");

    babelwires::TypeRef constructedTypeRef1(babelwires::LongIdentifier("Foo"), {babelwires::LongIdentifier("Bar")});
    EXPECT_EQ(constructedTypeRef1.serializeToString(), "Foo<Bar>");

    babelwires::TypeRef constructedTypeRef2(babelwires::LongIdentifier("Foo"),
                                            {babelwires::LongIdentifier("Bar"), babelwires::LongIdentifier("Flerm")});
    EXPECT_EQ(constructedTypeRef2.serializeToString(), "Foo<Bar,Flerm>");

    babelwires::TypeRef constructedTypeRef3(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"),
         babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")})});
    EXPECT_EQ(constructedTypeRef3.serializeToString(), "Foo<Bar,Flerm<Erm>>");
}

TEST(TypeRefTest, serializeToStringWithDiscriminators) {
    babelwires::TypeRef primitiveTypeRef(testUtils::getTestRegisteredLongIdentifier("Foo", 2));
    EXPECT_EQ(primitiveTypeRef.serializeToString(), "Foo`2");

    babelwires::TypeRef constructedTypeRef1(testUtils::getTestRegisteredLongIdentifier("Foo", 2),
                                            {testUtils::getTestRegisteredLongIdentifier("Bar", 4)});
    EXPECT_EQ(constructedTypeRef1.serializeToString(), "Foo`2<Bar`4>");

    babelwires::TypeRef constructedTypeRef2(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4), testUtils::getTestRegisteredLongIdentifier("Flerm", 1)});
    EXPECT_EQ(constructedTypeRef2.serializeToString(), "Foo`2<Bar`4,Flerm`1>");

    babelwires::TypeRef constructedTypeRef3(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4),
         babelwires::TypeRef(testUtils::getTestRegisteredLongIdentifier("Flerm", 1),
                             {testUtils::getTestRegisteredLongIdentifier("Erm", 13)})});
    EXPECT_EQ(constructedTypeRef3.serializeToString(), "Foo`2<Bar`4,Flerm`1<Erm`13>>");
}

TEST(TypeRefTest, deserializeFromStringNoDiscriminatorsSuccess) {
    babelwires::TypeRef primitiveTypeRef(babelwires::LongIdentifier("Foo"));
    EXPECT_EQ(primitiveTypeRef, babelwires::TypeRef::deserializeFromString("Foo"));

    babelwires::TypeRef constructedTypeRef1(babelwires::LongIdentifier("Foo"), {babelwires::LongIdentifier("Bar")});
    EXPECT_EQ(constructedTypeRef1, babelwires::TypeRef::deserializeFromString("Foo<Bar>"));

    babelwires::TypeRef constructedTypeRef2(babelwires::LongIdentifier("Foo"),
                                            {babelwires::LongIdentifier("Bar"), babelwires::LongIdentifier("Flerm")});
    EXPECT_EQ(constructedTypeRef2, babelwires::TypeRef::deserializeFromString("Foo<Bar,Flerm>"));

    babelwires::TypeRef constructedTypeRef3(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"),
         babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")})});
    EXPECT_EQ(constructedTypeRef3, babelwires::TypeRef::deserializeFromString("Foo<Bar,Flerm<Erm>>"));
}

TEST(TypeRefTest, deserializeFromStringWithDiscriminatorsSuccess) {
    EXPECT_EQ(babelwires::TypeRef(), babelwires::TypeRef::deserializeFromString("<>"));

    babelwires::TypeRef primitiveTypeRef(testUtils::getTestRegisteredLongIdentifier("Foo", 2));
    EXPECT_EQ(primitiveTypeRef, babelwires::TypeRef::deserializeFromString("Foo`2"));

    babelwires::TypeRef constructedTypeRef1(testUtils::getTestRegisteredLongIdentifier("Foo", 2),
                                            {testUtils::getTestRegisteredLongIdentifier("Bar", 4)});
    EXPECT_EQ(constructedTypeRef1, babelwires::TypeRef::deserializeFromString("Foo`2<Bar`4>"));

    babelwires::TypeRef constructedTypeRef2(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4), testUtils::getTestRegisteredLongIdentifier("Flerm", 1)});
    EXPECT_EQ(constructedTypeRef2, babelwires::TypeRef::deserializeFromString("Foo`2<Bar`4,Flerm`1>"));

    babelwires::TypeRef constructedTypeRef3(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4),
         babelwires::TypeRef(testUtils::getTestRegisteredLongIdentifier("Flerm", 1),
                             {testUtils::getTestRegisteredLongIdentifier("Erm", 13)})});
    EXPECT_EQ(constructedTypeRef3, babelwires::TypeRef::deserializeFromString("Foo`2<Bar`4,Flerm`1<Erm`13>>"));
}

TEST(TypeRefTest, deserializeFromStringFailure) {
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString(""), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("<"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString(">"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("<Foo>"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo<Bar"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo<Boo,"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo<,"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo,"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo>"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo<Bom>Flerm"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo Bee"), babelwires::ParseException);
}
