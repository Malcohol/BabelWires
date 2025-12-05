#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <execution>

TEST(TypeRefTest, equality) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef registeredTypeRef1(babelwires::RegisteredTypeId("Foo"));
    babelwires::TypeRef registeredTypeRef2(babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef2(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
                                            babelwires::RegisteredTypeId("Flerm"));
    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeRef constructedTypeRefValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeRef constructedTypeRefMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::RegisteredTypeId("Bar")}, {babelwires::IntValue(16)}});

    EXPECT_EQ(nullTypeRef, nullTypeRef);
    EXPECT_EQ(registeredTypeRef1, registeredTypeRef1);
    EXPECT_EQ(registeredTypeRef2, registeredTypeRef2);
    EXPECT_EQ(constructedTypeRef1, constructedTypeRef1);
    EXPECT_EQ(constructedTypeRef2, constructedTypeRef2);
    EXPECT_EQ(constructedTypeRef3, constructedTypeRef3);
    EXPECT_EQ(constructedTypeRefValue1, constructedTypeRefValue1);
    EXPECT_EQ(constructedTypeRefMixed1, constructedTypeRefMixed1);

    EXPECT_NE(nullTypeRef, registeredTypeRef1);
    EXPECT_NE(nullTypeRef, registeredTypeRef2);
    EXPECT_NE(nullTypeRef, constructedTypeRef1);
    EXPECT_NE(nullTypeRef, constructedTypeRef2);
    EXPECT_NE(nullTypeRef, constructedTypeRef3);
    EXPECT_NE(nullTypeRef, constructedTypeRefValue1);
    EXPECT_NE(nullTypeRef, constructedTypeRefMixed1);

    EXPECT_NE(registeredTypeRef1, registeredTypeRef2);
    EXPECT_NE(registeredTypeRef1, constructedTypeRef1);
    EXPECT_NE(registeredTypeRef1, constructedTypeRef2);
    EXPECT_NE(registeredTypeRef1, constructedTypeRef3);
    EXPECT_NE(registeredTypeRef1, constructedTypeRefValue1);
    EXPECT_NE(registeredTypeRef1, constructedTypeRefMixed1);

    EXPECT_NE(registeredTypeRef2, constructedTypeRef1);
    EXPECT_NE(registeredTypeRef2, constructedTypeRef2);
    EXPECT_NE(registeredTypeRef2, constructedTypeRef3);
    EXPECT_NE(registeredTypeRef2, constructedTypeRefValue1);
    EXPECT_NE(registeredTypeRef2, constructedTypeRefMixed1);

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

    const testUtils::TestType* testType = typeSystem.addEntry<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef typeRef(testUtils::TestType::getThisType());

    EXPECT_EQ(testType, &typeRef.resolve(typeSystem));
    babelwires::TypeRef constructedTypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                           testUtils::TestType::getThisType());
    const babelwires::Type& newType = constructedTypeRef.resolve(typeSystem);
    EXPECT_EQ(newType.getTypeRef(), constructedTypeRef);
    EXPECT_EQ(&constructedTypeRef.resolve(typeSystem), &constructedTypeRef.resolve(typeSystem));
}

TEST(TypeRefTest, tryResolveSuccess) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const testUtils::TestType* testType = typeSystem.addEntry<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef typeRef(testUtils::TestType::getThisType());
    EXPECT_EQ(testType, typeRef.tryResolve(typeSystem));

    babelwires::TypeRef constructedTypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                           testUtils::TestType::getThisType());
    const babelwires::Type* newType = constructedTypeRef.tryResolve(typeSystem);
    EXPECT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), constructedTypeRef);
    EXPECT_EQ(constructedTypeRef.tryResolve(typeSystem), constructedTypeRef.tryResolve(typeSystem));
}

TEST(TypeRefTest, tryResolveFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    EXPECT_EQ(nullptr, babelwires::TypeRef().tryResolve(typeSystem));
    EXPECT_EQ(nullptr, babelwires::TypeRef(babelwires::RegisteredTypeId("Foo")).tryResolve(typeSystem));
}

TEST(TypeRefTest, tryResolveParallel) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const testUtils::TestType* testType = typeSystem.addEntry<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef constructedTypeRef(
        testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
        babelwires::TypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                            babelwires::TypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                                testUtils::TestType::getThisType())));

    std::vector<std::tuple<babelwires::TypeRef, const babelwires::Type*>> vectorOfResolutions;
    for (int i = 0; i < 1000; ++i) {
        vectorOfResolutions.emplace_back(std::tuple{constructedTypeRef, nullptr});
    }
    std::for_each(
#ifndef __APPLE__
        std::execution::par,
#endif
        vectorOfResolutions.begin(), vectorOfResolutions.end(),
        [&typeSystem](auto& tuple) { std::get<1>(tuple) = std::get<0>(tuple).tryResolve(typeSystem); });
    for (int i = 0; i < 1000; ++i) {
        const babelwires::Type* const resolvedType = std::get<1>(vectorOfResolutions[i]);
        EXPECT_NE(resolvedType, nullptr);
        EXPECT_EQ(resolvedType->getTypeRef(), constructedTypeRef);
        EXPECT_EQ(resolvedType, std::get<1>(vectorOfResolutions[0]));
    }
}

TEST(TypeRefTest, tryResolveMixed) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const testUtils::TestType* testType = typeSystem.addEntry<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    const testUtils::TestMixedTypeConstructor* mixedConstructor =
        typeSystem.addTypeConstructor<testUtils::TestMixedTypeConstructor>();

    babelwires::TypeRef constructedTestTypeRef(
        testUtils::TestMixedTypeConstructor::getThisIdentifier(),
        {{babelwires::TypeRef(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                              testUtils::TestType::getThisType())},
         {babelwires::StringValue(" is this string")}});

    const babelwires::Type& constructedTestType = constructedTestTypeRef.resolve(typeSystem);
    const testUtils::TestType* const newTestType = constructedTestType.as<testUtils::TestType>();
    ASSERT_NE(newTestType, nullptr);
    EXPECT_EQ(newTestType->m_defaultValue, "Default value is this string");
}

TEST(TypeRefTest, toStringSuccessTypes) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    EXPECT_EQ(babelwires::TypeRef().toString(), "[]");

    babelwires::RegisteredTypeId foo = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Foo", "Foofoo", "00000000-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    EXPECT_EQ(babelwires::TypeRef(foo).toString(), "Foofoo");

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "UNARY[[{0}]]", "11111111-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary0, foo).toString(), "UNARY[Foofoo]");

    // Too many arguments is allowed.
    EXPECT_EQ(babelwires::TypeRef(unary0, foo, foo).toString(), "UNARY[Foofoo]");

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
        "Binary1", "}}{1}{{}}{0}[[", "44444444-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(binary1, foo, babelwires::TypeRef(unary0, foo)).toString(),
              "}UNARY[Foofoo]{}Foofoo[");
    EXPECT_EQ(babelwires::TypeRef(binary1, babelwires::TypeRef(unary0, foo), foo).toString(),
              "}Foofoo{}UNARY[Foofoo][");
    EXPECT_EQ(babelwires::TypeRef(unary0, babelwires::TypeRef(binary1, foo, foo)).toString(),
              "UNARY[}Foofoo{}Foofoo[]");

    babelwires::TypeConstructorId withOptional = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "TC<{0?none}>([0])", "971f582f-c6f9-4e46-9499-20f5d1d44cfd",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(withOptional, babelwires::TypeConstructorArguments{{foo}, {babelwires::IntValue(12)}}).toString(), "TC<Foofoo>(12)");
    EXPECT_EQ(babelwires::TypeRef(withOptional, babelwires::IntValue(12)).toString(), "TC<none>(12)");
    
    babelwires::TypeConstructorId withOp0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "{0|+}", "eebf69eb-bc91-44ce-8af6-8218f3d16705",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(withOp0, foo).toString(), "Foofoo");
    EXPECT_EQ(babelwires::TypeRef(withOp0, {{foo, foo}, {}}).toString(), "Foofoo+Foofoo");
    EXPECT_EQ(babelwires::TypeRef(withOp0, {{foo, foo, foo}, {}}).toString(), "Foofoo+Foofoo+Foofoo");

    babelwires::TypeConstructorId withOp1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "{0|<\\}>}", "7ce27b74-ba49-4c40-8c6a-c9b4cf09a005",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(withOp1, foo).toString(), "Foofoo");
    EXPECT_EQ(babelwires::TypeRef(withOp1, {{foo, foo}, {}}).toString(), "Foofoo<}>Foofoo");
    EXPECT_EQ(babelwires::TypeRef(withOp1, {{foo, foo, foo}, {}}).toString(), "Foofoo<}>Foofoo<}>Foofoo");

}

TEST(TypeRefTest, toStringSuccessValues) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::StringValue foo("foo");
    babelwires::StringValue bar("bar");

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "UNARY[[[0]]]", "8b3068ee-c604-4fa1-895f-c85dffeceda4",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary0, foo.clone()).toString(), "UNARY[foo]");

    // Too many arguments is allowed.
    EXPECT_EQ(babelwires::TypeRef(unary0, foo.clone(), foo.clone()).toString(), "UNARY[foo]");

    babelwires::TypeConstructorId binary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary0", "[0] + [1]", "48aaeb2e-6baf-427a-abf6-490e17da0978",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(binary0, foo.clone(), bar.clone()).toString(), "foo + bar");

    // With some escaped brackets.
    babelwires::TypeConstructorId binary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary1", "}}[1]{{]][0]{{", "51aa321f-6066-40c1-ad5e-a1d6c2f4fd00",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(binary1, foo.clone(), bar.clone()).toString(), "}bar{]foo{");

    babelwires::TypeConstructorId withOptional = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "TC([0?])", "971f582f-c6f9-4e46-9499-20f5d1d44cfd",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(withOptional, babelwires::TypeConstructorArguments{}).toString(), "TC()");
    EXPECT_EQ(babelwires::TypeRef(withOptional, babelwires::IntValue(12)).toString(), "TC(12)");

    babelwires::TypeConstructorId withOp0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "([0|,])", "896144d4-9972-4162-81c8-336869bb9cfc",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(withOp0, foo.clone()).toString(), "(foo)");
    EXPECT_EQ(babelwires::TypeRef(withOp0, {{}, {foo.clone(), bar.clone()}}).toString(), "(foo,bar)");
    EXPECT_EQ(babelwires::TypeRef(withOp0, {{}, {foo.clone(), bar.clone(), foo.clone()}}).toString(), "(foo,bar,foo)");

    babelwires::TypeConstructorId withOp1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "[0|<\\]>]", "393cda51-e0a1-4610-85e6-2dbffa971225",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(withOp1, foo.clone()).toString(), "foo");
    EXPECT_EQ(babelwires::TypeRef(withOp1, {{}, {foo.clone(), bar.clone()}}).toString(), "foo<]>bar");
    EXPECT_EQ(babelwires::TypeRef(withOp1, {{}, {foo.clone(), bar.clone(), foo.clone()}}).toString(), "foo<]>bar<]>foo");
}

TEST(TypeRefTest, toStringSuccessMixed) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::RegisteredTypeId foo = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Foo", "Foofoo", "325d9560-6330-43bb-80b4-963843ec8fba",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    babelwires::StringValue bar("bar");
    babelwires::StringValue boo("boo");

    babelwires::TypeConstructorId mixed = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "{0}+[0|<\\]>]+{1}", "258495b5-a88e-45c0-83ca-762817c27fb1",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(mixed, {{foo, foo}, {bar.clone(), boo.clone(), bar.clone()}}).toString(), "Foofoo+bar<]>boo<]>bar+Foofoo");
}

TEST(TypeRefTest, toStringMalformed) {
    // The format string can be read from data.
    // ToString should not assert or throw since it might be used in displaying log or error messages.
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::RegisteredTypeId foo = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Foo", "Foofoo", "00000000-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "{", "11111111-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary0, foo).toString(), "MalformedTypeRef");

    babelwires::TypeConstructorId unary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary1", "oo{", "22222222-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary1, foo).toString(), "MalformedTypeRef");

    // This type of format string is not supported.
    babelwires::TypeConstructorId unary2 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary2", "oo{}pp", "33333333-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(unary2, foo).toString(), "MalformedTypeRef");

    // This type of format string is not supported.
    babelwires::TypeConstructorId valueBroken = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "ValueBroken", "oo[pp", "18d028cb-de11-4974-86bf-068be2a509fd",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeRef(valueBroken, foo).toString(), "MalformedTypeRef");
}

TEST(TypeRefTest, serialization) {
    babelwires::TypeRef nullTypeRef;
    babelwires::TypeRef registeredTypeRef1(babelwires::RegisteredTypeId("Foo"));
    babelwires::TypeRef registeredTypeRef2(babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef2(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
                                            babelwires::RegisteredTypeId("Flerm"));
    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeRef constructedTypeRefValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeRef constructedTypeRefMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::RegisteredTypeId("Bar")}, {babelwires::IntValue(16)}});

    std::vector<babelwires::TypeRef> testRefs = {
        nullTypeRef,         registeredTypeRef1,   registeredTypeRef2,        constructedTypeRef1,
        constructedTypeRef2, constructedTypeRef3, constructedTypeRefValue1, constructedTypeRefMixed1};

    for (auto typeRef : testRefs) {
        std::string serializedContents;
        {
            babelwires::XmlSerializer serializer;
            serializer.serializeObject(typeRef);
            std::ostringstream os;
            serializer.write(os);
            serializedContents = std::move(os.str());
        }
        testUtils::TestLog log;
        babelwires::AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto typeRefPtr = deserializer.deserializeObject<babelwires::TypeRef>();
        deserializer.finalize();

        ASSERT_NE(typeRefPtr, nullptr);
        EXPECT_EQ(*typeRefPtr, typeRef);
    }
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
    babelwires::TypeRef registeredTypeRef1(babelwires::RegisteredTypeId("Foo"));
    babelwires::TypeRef registeredTypeRef2(babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef1(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeRef constructedTypeRef2(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Flerm"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeRef constructedTypeRef3(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeRef(babelwires::TypeConstructorId("Oom"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeRef constructedTypeRefValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeRef constructedTypeRefValue2(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Erm"));
    babelwires::TypeRef constructedTypeRefMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::RegisteredTypeId("Bar")}, {babelwires::IntValue(16)}});

    std::hash<babelwires::TypeRef> hasher;

    // In theory, some of these could fail due to a hash collision.
    EXPECT_NE(hasher(nullTypeRef), hasher(registeredTypeRef1));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(registeredTypeRef2));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(constructedTypeRef1));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(constructedTypeRef2));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(constructedTypeRef3));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(constructedTypeRefValue1));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(constructedTypeRefValue2));
    EXPECT_NE(hasher(registeredTypeRef1), hasher(constructedTypeRefMixed1));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRef2));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRef3));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRefValue1));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRefValue2));
    EXPECT_NE(hasher(constructedTypeRef1), hasher(constructedTypeRefMixed1));
    EXPECT_NE(hasher(constructedTypeRef2), hasher(constructedTypeRef3));
    EXPECT_NE(hasher(constructedTypeRef2), hasher(constructedTypeRef3));
    EXPECT_NE(hasher(constructedTypeRef2), hasher(constructedTypeRefValue1));
    EXPECT_NE(hasher(constructedTypeRef2), hasher(constructedTypeRefValue2));
    EXPECT_NE(hasher(constructedTypeRef2), hasher(constructedTypeRefMixed1));
    EXPECT_NE(hasher(constructedTypeRefValue1), hasher(constructedTypeRefValue2));
    EXPECT_NE(hasher(constructedTypeRefValue1), hasher(constructedTypeRefMixed1));
    EXPECT_NE(hasher(constructedTypeRefValue2), hasher(constructedTypeRefMixed1));
}