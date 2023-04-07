#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <execution>

TEST(TypeRefTest, equality) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef primitiveTypeRef1(babelwires::PrimitiveTypeId("Foo"));
    babelwires::TypeRef primitiveTypeRef2(babelwires::PrimitiveTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef2(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
                                            babelwires::PrimitiveTypeId("Flerm"));
    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::PrimitiveTypeId("Erm")));
    babelwires::TypeRef constructedTypeRefValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeRef constructedTypeRefMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::PrimitiveTypeId("Bar")}, {babelwires::IntValue(16)}});

    EXPECT_EQ(nullTypeRef, nullTypeRef);
    EXPECT_EQ(primitiveTypeRef1, primitiveTypeRef1);
    EXPECT_EQ(primitiveTypeRef2, primitiveTypeRef2);
    EXPECT_EQ(constructedTypeRef1, constructedTypeRef1);
    EXPECT_EQ(constructedTypeRef2, constructedTypeRef2);
    EXPECT_EQ(constructedTypeRef3, constructedTypeRef3);
    EXPECT_EQ(constructedTypeRefValue1, constructedTypeRefValue1);
    EXPECT_EQ(constructedTypeRefMixed1, constructedTypeRefMixed1);

    EXPECT_NE(nullTypeRef, primitiveTypeRef1);
    EXPECT_NE(nullTypeRef, primitiveTypeRef2);
    EXPECT_NE(nullTypeRef, constructedTypeRef1);
    EXPECT_NE(nullTypeRef, constructedTypeRef2);
    EXPECT_NE(nullTypeRef, constructedTypeRef3);
    EXPECT_NE(nullTypeRef, constructedTypeRefValue1);
    EXPECT_NE(nullTypeRef, constructedTypeRefMixed1);

    EXPECT_NE(primitiveTypeRef1, primitiveTypeRef2);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRef1);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRef2);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRef3);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRefValue1);
    EXPECT_NE(primitiveTypeRef1, constructedTypeRefMixed1);

    EXPECT_NE(primitiveTypeRef2, constructedTypeRef1);
    EXPECT_NE(primitiveTypeRef2, constructedTypeRef2);
    EXPECT_NE(primitiveTypeRef2, constructedTypeRef3);
    EXPECT_NE(primitiveTypeRef2, constructedTypeRefValue1);
    EXPECT_NE(primitiveTypeRef2, constructedTypeRefMixed1);

    EXPECT_NE(constructedTypeRef1, constructedTypeRef2);
    EXPECT_NE(constructedTypeRef1, constructedTypeRef3);
    EXPECT_NE(constructedTypeRef1, constructedTypeRefValue1);
    EXPECT_NE(constructedTypeRef1, constructedTypeRefMixed1);

    EXPECT_NE(constructedTypeRef2, constructedTypeRef3);
    EXPECT_NE(constructedTypeRef2, constructedTypeRefValue1);
    EXPECT_NE(constructedTypeRef2, constructedTypeRefMixed1);

    EXPECT_NE(constructedTypeRefValue1, constructedTypeRefMixed1);
}

TEST(TypeRefTest, resolve) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const testUtils::TestEnum* testEnum = typeSystem.addEntry<testUtils::TestEnum>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef typeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_EQ(testEnum, &typeRef.resolve(typeSystem));
    babelwires::TypeRef constructedTypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                           testUtils::TestEnum::getThisIdentifier());
    const babelwires::Type& newType = constructedTypeRef.resolve(typeSystem);
    EXPECT_EQ(newType.getTypeRef(), constructedTypeRef);
    EXPECT_EQ(&constructedTypeRef.resolve(typeSystem), &constructedTypeRef.resolve(typeSystem));
}

TEST(TypeRefTest, tryResolveSuccess) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const testUtils::TestEnum* testEnum = typeSystem.addEntry<testUtils::TestEnum>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef typeRef(testUtils::TestEnum::getThisIdentifier());
    EXPECT_EQ(testEnum, typeRef.tryResolve(typeSystem));

    babelwires::TypeRef constructedTypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                           testUtils::TestEnum::getThisIdentifier());
    const babelwires::Type* newType = constructedTypeRef.tryResolve(typeSystem);
    EXPECT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), constructedTypeRef);
    EXPECT_EQ(constructedTypeRef.tryResolve(typeSystem), constructedTypeRef.tryResolve(typeSystem));
}

TEST(TypeRefTest, tryResolveFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    EXPECT_EQ(nullptr, babelwires::TypeRef().tryResolve(typeSystem));
    EXPECT_EQ(nullptr, babelwires::TypeRef(babelwires::PrimitiveTypeId("Foo")).tryResolve(typeSystem));
}

TEST(TypeRefTest, tryResolveParallel) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const testUtils::TestEnum* testEnum = typeSystem.addEntry<testUtils::TestEnum>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef constructedTypeRef(
        testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
        babelwires::TypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                            babelwires::TypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                                testUtils::TestEnum::getThisIdentifier())));

    std::vector<std::tuple<babelwires::TypeRef, const babelwires::Type*>> vectorOfResolutions;
    for (int i = 0; i < 1000; ++i) {
        vectorOfResolutions.emplace_back(std::tuple{constructedTypeRef, nullptr});
    }
    std::for_each(std::execution::par, vectorOfResolutions.begin(), vectorOfResolutions.end(),
                  [&typeSystem](auto& tuple) { std::get<1>(tuple) = std::get<0>(tuple).tryResolve(typeSystem); });
    for (int i = 0; i < 1000; ++i) {
        const babelwires::Type* const resolvedType = std::get<1>(vectorOfResolutions[i]);
        EXPECT_NE(resolvedType, nullptr);
        EXPECT_EQ(resolvedType->getTypeRef(), constructedTypeRef);
        EXPECT_EQ(resolvedType, std::get<1>(vectorOfResolutions[0]));
    }
}

TEST(TypeRefTest, toStringSuccess) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    EXPECT_EQ(babelwires::TypeRef().toString(), "[]");

    babelwires::PrimitiveTypeId foo = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Foo", "Foofoo", "00000000-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    EXPECT_EQ(babelwires::TypeRef(foo).toString(), "Foofoo");

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "UNARY[{0}]", "11111111-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary0, foo).toString(), "UNARY[Foofoo]");
    EXPECT_EQ(babelwires::TypeRef(unary0, babelwires::TypeRef(unary0, foo)).toString(), "UNARY[UNARY[Foofoo]]");

    babelwires::TypeConstructorId unary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary1", "{0}++", "22222222-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary1, foo).toString(), "Foofoo++");
    EXPECT_EQ(babelwires::TypeRef(unary1, babelwires::TypeRef(unary0, foo)).toString(), "UNARY[Foofoo]++");
    EXPECT_EQ(babelwires::TypeRef(unary0, babelwires::TypeRef(unary1, foo)).toString(), "UNARY[Foofoo++]");

    babelwires::TypeConstructorId binary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary0", "{0} + {1}", "33333333-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(binary0, foo, babelwires::TypeRef(unary0, foo)).toString(), "Foofoo + UNARY[Foofoo]");
    EXPECT_EQ(babelwires::TypeRef(binary0, babelwires::TypeRef(unary0, foo), foo).toString(), "UNARY[Foofoo] + Foofoo");
    EXPECT_EQ(babelwires::TypeRef(unary0, babelwires::TypeRef(binary0, foo, foo)).toString(), "UNARY[Foofoo + Foofoo]");

    // With some escaped brackets.
    babelwires::TypeConstructorId binary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary1", "}}{1}{{}}{0}{{", "44444444-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(binary1, foo, babelwires::TypeRef(unary0, foo)).toString(),
              "}UNARY[Foofoo]{}Foofoo{");
    EXPECT_EQ(babelwires::TypeRef(binary1, babelwires::TypeRef(unary0, foo), foo).toString(),
              "}Foofoo{}UNARY[Foofoo]{");
    EXPECT_EQ(babelwires::TypeRef(unary0, babelwires::TypeRef(binary1, foo, foo)).toString(),
              "UNARY[}Foofoo{}Foofoo{]");
}

TEST(TypeRefTest, toStringMalformed) {
    // The format string can be read from data.
    // ToString should not assert or throw since it might be used in displaying log or error messages.
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::PrimitiveTypeId foo = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Foo", "Foofoo", "00000000-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "{", "11111111-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary0, foo).toString(), "MalformedTypeRef{Unary0'1[Foo'1]}");

    babelwires::TypeConstructorId unary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary1", "oo{", "22222222-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary1, foo).toString(), "MalformedTypeRef{Unary1'1[Foo'1]}");

    // This type of format string is not supported.
    babelwires::TypeConstructorId unary2 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary2", "oo{}pp", "33333333-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary2, foo).toString(), "MalformedTypeRef{Unary2'1[Foo'1]}");

    babelwires::TypeConstructorId unary3 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary3", "UNARY{0}", "44444444-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    // Too many arguments
    EXPECT_EQ(babelwires::TypeRef(unary3, foo, foo).toString(), "MalformedTypeRef{Unary3'1[Foo'1,Foo'1]}");
}

TEST(TypeRefTest, serializeToStringNoDiscriminators) {
    babelwires::TypeRef nullTypeRef;
    EXPECT_EQ(nullTypeRef.serializeToString(), "[]");

    babelwires::TypeRef primitiveTypeRef(babelwires::PrimitiveTypeId("Foo"));
    EXPECT_EQ(primitiveTypeRef.serializeToString(), "Foo");

    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"));
    EXPECT_EQ(constructedTypeRef1.serializeToString(), "Foo[Bar]");

    babelwires::TypeRef constructedTypeRef2(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
                                            babelwires::PrimitiveTypeId("Flerm"));
    EXPECT_EQ(constructedTypeRef2.serializeToString(), "Foo[Bar,Flerm]");

    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::PrimitiveTypeId("Erm")));
    EXPECT_EQ(constructedTypeRef3.serializeToString(), "Foo[Bar,Flerm[Erm]]");

    babelwires::TypeRef constructedTypeRef4(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::PrimitiveTypeId("Erm")),
        babelwires::PrimitiveTypeId("Bar"));
    EXPECT_EQ(constructedTypeRef4.serializeToString(), "Foo[Flerm[Erm],Bar]");
}

TEST(TypeRefTest, serializeToStringWithDiscriminators) {
    babelwires::TypeRef primitiveTypeRef(testUtils::getTestRegisteredMediumIdentifier("Foo", 2));
    EXPECT_EQ(primitiveTypeRef.serializeToString(), "Foo'2");

    babelwires::TypeRef constructedTypeRef1(testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
                                            testUtils::getTestRegisteredMediumIdentifier("Bar", 4));
    EXPECT_EQ(constructedTypeRef1.serializeToString(), "Foo'2[Bar'4]");

    babelwires::TypeRef constructedTypeRef2(testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
                                            testUtils::getTestRegisteredMediumIdentifier("Bar", 4),
                                            testUtils::getTestRegisteredMediumIdentifier("Flerm", 1));
    EXPECT_EQ(constructedTypeRef2.serializeToString(), "Foo'2[Bar'4,Flerm'1]");

    babelwires::TypeRef constructedTypeRef3(
        testUtils::getTestRegisteredMediumIdentifier("Foo", 2), testUtils::getTestRegisteredMediumIdentifier("Bar", 4),
        babelwires::TypeRef(testUtils::getTestRegisteredMediumIdentifier("Flerm", 1),
                            testUtils::getTestRegisteredMediumIdentifier("Erm", 13)));
    EXPECT_EQ(constructedTypeRef3.serializeToString(), "Foo'2[Bar'4,Flerm'1[Erm'13]]");

    babelwires::TypeRef constructedTypeRef4(
        testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
        babelwires::TypeRef(testUtils::getTestRegisteredMediumIdentifier("Flerm", 1),
                            testUtils::getTestRegisteredMediumIdentifier("Erm", 13)),
        testUtils::getTestRegisteredMediumIdentifier("Bar", 4));
    EXPECT_EQ(constructedTypeRef4.serializeToString(), "Foo'2[Flerm'1[Erm'13],Bar'4]");
}

TEST(TypeRefTest, deserializeFromStringNoDiscriminatorsSuccess) {
    babelwires::TypeRef primitiveTypeRef(babelwires::TypeConstructorId("Foo"));
    EXPECT_EQ(primitiveTypeRef, babelwires::TypeRef::deserializeFromString("Foo"));

    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"));
    EXPECT_EQ(constructedTypeRef1, babelwires::TypeRef::deserializeFromString("Foo[Bar]"));

    babelwires::TypeRef constructedTypeRef2(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
                                            babelwires::PrimitiveTypeId("Flerm"));
    EXPECT_EQ(constructedTypeRef2, babelwires::TypeRef::deserializeFromString("Foo[Bar,Flerm]"));

    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::PrimitiveTypeId("Erm")));
    EXPECT_EQ(constructedTypeRef3, babelwires::TypeRef::deserializeFromString("Foo[Bar,Flerm[Erm]]"));

    babelwires::TypeRef constructedTypeRef4(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::PrimitiveTypeId("Erm")),
        babelwires::PrimitiveTypeId("Bar"));
    EXPECT_EQ(constructedTypeRef4, babelwires::TypeRef::deserializeFromString("Foo[Flerm[Erm],Bar]"));
}

TEST(TypeRefTest, deserializeFromStringWithDiscriminatorsSuccess) {
    EXPECT_EQ(babelwires::TypeRef(), babelwires::TypeRef::deserializeFromString("[]"));

    babelwires::TypeRef primitiveTypeRef(testUtils::getTestRegisteredMediumIdentifier("Foo", 2));
    EXPECT_EQ(primitiveTypeRef, babelwires::TypeRef::deserializeFromString("Foo'2"));

    babelwires::TypeRef constructedTypeRef1(testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
                                            testUtils::getTestRegisteredMediumIdentifier("Bar", 4));
    EXPECT_EQ(constructedTypeRef1, babelwires::TypeRef::deserializeFromString("Foo'2[Bar'4]"));

    babelwires::TypeRef constructedTypeRef2(testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
                                            testUtils::getTestRegisteredMediumIdentifier("Bar", 4),
                                            testUtils::getTestRegisteredMediumIdentifier("Flerm", 1));
    EXPECT_EQ(constructedTypeRef2, babelwires::TypeRef::deserializeFromString("Foo'2[Bar'4,Flerm'1]"));

    babelwires::TypeRef constructedTypeRef3(
        testUtils::getTestRegisteredMediumIdentifier("Foo", 2), testUtils::getTestRegisteredMediumIdentifier("Bar", 4),
        babelwires::TypeRef(testUtils::getTestRegisteredMediumIdentifier("Flerm", 1),
                            testUtils::getTestRegisteredMediumIdentifier("Erm", 13)));
    EXPECT_EQ(constructedTypeRef3, babelwires::TypeRef::deserializeFromString("Foo'2[Bar'4,Flerm'1[Erm'13]]"));

    babelwires::TypeRef constructedTypeRef4(
        testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
        babelwires::TypeRef(testUtils::getTestRegisteredMediumIdentifier("Flerm", 1),
                            testUtils::getTestRegisteredMediumIdentifier("Erm", 13)),
        testUtils::getTestRegisteredMediumIdentifier("Bar", 4));
    EXPECT_EQ(constructedTypeRef4, babelwires::TypeRef::deserializeFromString("Foo'2[Flerm'1[Erm'13],Bar'4]"));
}

TEST(TypeRefTest, deserializeFromStringFailure) {
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString(""), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("["), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("]"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("[Foo]"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo[Bar"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo[Boo,"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo[,"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo,"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo]"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo[Bom]Flerm"), babelwires::ParseException);
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo Bee"), babelwires::ParseException);
    // Too many arguments
    EXPECT_THROW(babelwires::TypeRef::deserializeFromString("Foo[A,B,C,D,E,F,G,H,I,J,K]"), babelwires::ParseException);
}

TEST(TypeRefTest, visitIdentifiers) {
    babelwires::TypeRef typeRef(testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
                                testUtils::getTestRegisteredMediumIdentifier("Bar", 4),
                                babelwires::TypeRef(testUtils::getTestRegisteredMediumIdentifier("Flerm", 1),
                                                    testUtils::getTestRegisteredMediumIdentifier("Erm", 13)));

    struct Visitor : babelwires::IdentifierVisitor {
        void operator()(babelwires::ShortId& identifier) {
            m_seen.emplace(identifier);
            identifier.setDiscriminator(17);
        }
        void operator()(babelwires::MediumId& identifier) {
            m_seen.emplace(identifier);
            identifier.setDiscriminator(18);
        }
        void operator()(babelwires::LongId& identifier) {
            m_seen.emplace(identifier);
            identifier.setDiscriminator(24);
        }
        std::set<babelwires::ShortId> m_seen;
    } visitor1, visitor2;

    typeRef.visitIdentifiers(visitor1);
    typeRef.visitIdentifiers(visitor2);

    auto it = visitor1.m_seen.begin();
    EXPECT_EQ(*it, "Bar");
    EXPECT_EQ(it->getDiscriminator(), 4);
    ++it;
    EXPECT_EQ(*it, "Erm");
    EXPECT_EQ(it->getDiscriminator(), 13);
    ++it;
    EXPECT_EQ(*it, "Flerm");
    EXPECT_EQ(it->getDiscriminator(), 1);
    ++it;
    EXPECT_EQ(*it, "Foo");
    EXPECT_EQ(it->getDiscriminator(), 2);
    ++it;
    EXPECT_EQ(it, visitor1.m_seen.end());

    for (auto id : visitor2.m_seen) {
        EXPECT_EQ(id.getDiscriminator(), 18);
    }
}

TEST(TypeRefTest, hash) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef primitiveTypeRef1(babelwires::PrimitiveTypeId("Foo"));
    babelwires::TypeRef primitiveTypeRef2(babelwires::PrimitiveTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef2(
        babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::PrimitiveTypeId("Erm")));
    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::PrimitiveTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Oom"), babelwires::PrimitiveTypeId("Erm")));

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