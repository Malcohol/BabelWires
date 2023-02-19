#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(TypeRefTest, serializeToStringNoDiscriminators) {
    babelwires::TypeRef nullTypeRef;
    EXPECT_EQ(nullTypeRef.serializeToString(), "<>");

    babelwires::TypeRef primitiveTypeRef(babelwires::LongIdentifier("Foo"));
    EXPECT_EQ(primitiveTypeRef.serializeToString(), "Foo");

    babelwires::TypeRef constructedTypeRef1(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar")});
    EXPECT_EQ(constructedTypeRef1.serializeToString(), "Foo<Bar>");

    babelwires::TypeRef constructedTypeRef2(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"), babelwires::LongIdentifier("Flerm")});
    EXPECT_EQ(constructedTypeRef2.serializeToString(), "Foo<Bar,Flerm>");
}

TEST(TypeRefTest, serializeToStringWithDiscriminators) {
    babelwires::TypeRef primitiveTypeRef(testUtils::getTestRegisteredLongIdentifier("Foo", 2));
    EXPECT_EQ(primitiveTypeRef.serializeToString(), "Foo`2");

    babelwires::TypeRef constructedTypeRef1(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4)});
    EXPECT_EQ(constructedTypeRef1.serializeToString(), "Foo`2<Bar`4>");

    babelwires::TypeRef constructedTypeRef2(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4), testUtils::getTestRegisteredLongIdentifier("Flerm", 1)});
    EXPECT_EQ(constructedTypeRef2.serializeToString(), "Foo`2<Bar`4,Flerm`1>");
}

TEST(TypeRefTest, deserializeFromStringNoDiscriminatorsSuccess) {
    babelwires::TypeRef primitiveTypeRef(babelwires::LongIdentifier("Foo"));
    EXPECT_EQ(primitiveTypeRef, babelwires::TypeRef::deserializeFromString("Foo"));

    babelwires::TypeRef constructedTypeRef1(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar")});
    EXPECT_EQ(constructedTypeRef1, babelwires::TypeRef::deserializeFromString("Foo<Bar>"));

    babelwires::TypeRef constructedTypeRef2(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"), babelwires::LongIdentifier("Flerm")});
    EXPECT_EQ(constructedTypeRef2, babelwires::TypeRef::deserializeFromString("Foo<Bar,Flerm>"));
}

TEST(TypeRefTest, deserializeFromStringWithDiscriminatorsSuccess) {
    EXPECT_EQ(babelwires::TypeRef(), babelwires::TypeRef::deserializeFromString("<>"));

    babelwires::TypeRef primitiveTypeRef(testUtils::getTestRegisteredLongIdentifier("Foo", 2));
    EXPECT_EQ(primitiveTypeRef, babelwires::TypeRef::deserializeFromString("Foo`2"));

    babelwires::TypeRef constructedTypeRef1(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4)});
    EXPECT_EQ(constructedTypeRef1, babelwires::TypeRef::deserializeFromString("Foo`2<Bar`4>"));

    babelwires::TypeRef constructedTypeRef2(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {testUtils::getTestRegisteredLongIdentifier("Bar", 4), testUtils::getTestRegisteredLongIdentifier("Flerm", 1)});
    EXPECT_EQ(constructedTypeRef2, babelwires::TypeRef::deserializeFromString("Foo`2<Bar`4,Flerm`1>"));
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

