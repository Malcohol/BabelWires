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

TEST(TypeRefTest, lessThan) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef primitiveTypeRef1(babelwires::LongIdentifier("Bar"));
    babelwires::TypeRef primitiveTypeRef2(babelwires::LongIdentifier("Foo"));
    babelwires::TypeRef constructedTypeRef1(babelwires::LongIdentifier("Foo"), {babelwires::LongIdentifier("Bar")});
    babelwires::TypeRef constructedTypeRef2(babelwires::LongIdentifier("Foo"),
                                            {babelwires::LongIdentifier("Bar"), babelwires::LongIdentifier("Flerm")});
    babelwires::TypeRef constructedTypeRef3(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"),
         babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")})});
    babelwires::TypeRef constructedTypeRef4(babelwires::LongIdentifier("Foo"), {babelwires::LongIdentifier("Boo")});

    EXPECT_LT(nullTypeRef, primitiveTypeRef1);
    EXPECT_LT(primitiveTypeRef1, primitiveTypeRef2);
    EXPECT_LT(primitiveTypeRef1, constructedTypeRef1);
    EXPECT_LT(constructedTypeRef1, constructedTypeRef2);
    EXPECT_LT(constructedTypeRef1, constructedTypeRef3);
    EXPECT_LT(constructedTypeRef1, constructedTypeRef4);
    EXPECT_LT(constructedTypeRef2, constructedTypeRef3);
    EXPECT_LT(constructedTypeRef2, constructedTypeRef4);
    EXPECT_LT(constructedTypeRef3, constructedTypeRef4);

    EXPECT_FALSE(nullTypeRef < nullTypeRef);
    EXPECT_FALSE(primitiveTypeRef1 < primitiveTypeRef1);
    EXPECT_FALSE(primitiveTypeRef2 < primitiveTypeRef2);
    EXPECT_FALSE(constructedTypeRef1 < constructedTypeRef1);
    EXPECT_FALSE(constructedTypeRef2 < constructedTypeRef2);
    EXPECT_FALSE(constructedTypeRef3 < constructedTypeRef3);
    EXPECT_FALSE(constructedTypeRef4 < constructedTypeRef4);

    EXPECT_FALSE(primitiveTypeRef1 < nullTypeRef);
    EXPECT_FALSE(primitiveTypeRef2 < primitiveTypeRef1);
    EXPECT_FALSE(constructedTypeRef1 < primitiveTypeRef1);
    EXPECT_FALSE(constructedTypeRef2 < constructedTypeRef1);
    EXPECT_FALSE(constructedTypeRef3 < constructedTypeRef1);
    EXPECT_FALSE(constructedTypeRef4 < constructedTypeRef1);
    EXPECT_FALSE(constructedTypeRef3 < constructedTypeRef2);
    EXPECT_FALSE(constructedTypeRef4 < constructedTypeRef2);
    EXPECT_FALSE(constructedTypeRef4 < constructedTypeRef3);
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

    babelwires::TypeRef constructedTypeRef4(
        babelwires::LongIdentifier("Foo"),
        {babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")}),
         babelwires::LongIdentifier("Bar")});
    EXPECT_EQ(constructedTypeRef4.serializeToString(), "Foo<Flerm<Erm>,Bar>");
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

    babelwires::TypeRef constructedTypeRef4(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {babelwires::TypeRef(testUtils::getTestRegisteredLongIdentifier("Flerm", 1),
                             {testUtils::getTestRegisteredLongIdentifier("Erm", 13)}),
         testUtils::getTestRegisteredLongIdentifier("Bar", 4)});
    EXPECT_EQ(constructedTypeRef4.serializeToString(), "Foo`2<Flerm`1<Erm`13>,Bar`4>");
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

    babelwires::TypeRef constructedTypeRef4(
        babelwires::LongIdentifier("Foo"),
        {babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")}),
         babelwires::LongIdentifier("Bar")});
    EXPECT_EQ(constructedTypeRef4, babelwires::TypeRef::deserializeFromString("Foo<Flerm<Erm>,Bar>"));
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

    babelwires::TypeRef constructedTypeRef4(
        testUtils::getTestRegisteredLongIdentifier("Foo", 2),
        {
            babelwires::TypeRef(testUtils::getTestRegisteredLongIdentifier("Flerm", 1),
                                {testUtils::getTestRegisteredLongIdentifier("Erm", 13)}),
            testUtils::getTestRegisteredLongIdentifier("Bar", 4),
        });
    EXPECT_EQ(constructedTypeRef4, babelwires::TypeRef::deserializeFromString("Foo`2<Flerm`1<Erm`13>,Bar`4>"));
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

TEST(TypeRefTest, visitIdentifiers) {
    babelwires::TypeRef typeRef(testUtils::getTestRegisteredLongIdentifier("Foo", 2),
                                {testUtils::getTestRegisteredLongIdentifier("Bar", 4),
                                 babelwires::TypeRef(testUtils::getTestRegisteredLongIdentifier("Flerm", 1),
                                                     {testUtils::getTestRegisteredLongIdentifier("Erm", 13)})});

    struct Visitor : babelwires::IdentifierVisitor {
        void operator()(babelwires::Identifier& identifier) { m_seen.emplace(identifier); identifier.setDiscriminator(17); }
        void operator()(babelwires::LongIdentifier& identifier) { m_seen.emplace(identifier); identifier.setDiscriminator(18); }
        std::set<babelwires::Identifier> m_seen;
    } visitor1, visitor2;

    typeRef.visitIdentifiers(visitor1);
    typeRef.visitIdentifiers(visitor2);

    auto it = visitor1.m_seen.begin();
    EXPECT_EQ(*it, "Bar"); EXPECT_EQ(it->getDiscriminator(), 4); ++it;
    EXPECT_EQ(*it, "Erm"); EXPECT_EQ(it->getDiscriminator(), 13); ++it;
    EXPECT_EQ(*it, "Flerm"); EXPECT_EQ(it->getDiscriminator(), 1); ++it;
    EXPECT_EQ(*it, "Foo"); EXPECT_EQ(it->getDiscriminator(), 2); ++it;
    EXPECT_EQ(it, visitor1.m_seen.end());

    for(auto id : visitor2.m_seen) {
        EXPECT_EQ(id.getDiscriminator(), 18);
    }
}

TEST(TypeRefTest, hash) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef primitiveTypeRef1(babelwires::LongIdentifier("Foo"));
    babelwires::TypeRef primitiveTypeRef2(babelwires::LongIdentifier("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::LongIdentifier("Foo"), {babelwires::LongIdentifier("Bar")});
    babelwires::TypeRef constructedTypeRef2(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"),
         babelwires::TypeRef(babelwires::LongIdentifier("Flerm"), {babelwires::LongIdentifier("Erm")})});   
    babelwires::TypeRef constructedTypeRef3(
        babelwires::LongIdentifier("Foo"),
        {babelwires::LongIdentifier("Bar"),
         babelwires::TypeRef(babelwires::LongIdentifier("Oom"), {babelwires::LongIdentifier("Erm")})});   

    std::hash<babelwires::TypeRef> hasher;

    // In theory, some of these could fail due to a hash collision.
    EXPECT_NE(hasher(nullTypeRef), hasher(primitiveTypeRef1));
    EXPECT_NE(hasher(primitiveTypeRef1), hasher(primitiveTypeRef2));
    EXPECT_NE(hasher(primitiveTypeRef1), hasher(constructedTypeRef1));
    EXPECT_NE(hasher(primitiveTypeRef1), hasher(constructedTypeRef2));
    EXPECT_NE(hasher(primitiveTypeRef1), hasher(constructedTypeRef3));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRef2));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRef3));
    EXPECT_NE(hasher(constructedTypeRef2), hasher(constructedTypeRef3));
}