#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <algorithm>
#include <execution>
#include <random>

TEST(TypeExpTest, equality) {
    babelwires::TypeExp nullTypeExp;
    babelwires::TypeExp registeredTypeExp1(babelwires::RegisteredTypeId("Foo"));
    babelwires::TypeExp registeredTypeExp2(babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeExp constructedTypeExp1(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeExp constructedTypeExp2(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
                                            babelwires::RegisteredTypeId("Flerm"));
    babelwires::TypeExp constructedTypeExp3(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeExp(babelwires::TypeConstructorId("Flerm"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeExp constructedTypeExpValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeExp constructedTypeExpMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::RegisteredTypeId("Bar")}, {babelwires::IntValue(16)}});

    EXPECT_EQ(nullTypeExp, nullTypeExp);
    EXPECT_EQ(registeredTypeExp1, registeredTypeExp1);
    EXPECT_EQ(registeredTypeExp2, registeredTypeExp2);
    EXPECT_EQ(constructedTypeExp1, constructedTypeExp1);
    EXPECT_EQ(constructedTypeExp2, constructedTypeExp2);
    EXPECT_EQ(constructedTypeExp3, constructedTypeExp3);
    EXPECT_EQ(constructedTypeExpValue1, constructedTypeExpValue1);
    EXPECT_EQ(constructedTypeExpMixed1, constructedTypeExpMixed1);

    EXPECT_NE(nullTypeExp, registeredTypeExp1);
    EXPECT_NE(nullTypeExp, registeredTypeExp2);
    EXPECT_NE(nullTypeExp, constructedTypeExp1);
    EXPECT_NE(nullTypeExp, constructedTypeExp2);
    EXPECT_NE(nullTypeExp, constructedTypeExp3);
    EXPECT_NE(nullTypeExp, constructedTypeExpValue1);
    EXPECT_NE(nullTypeExp, constructedTypeExpMixed1);

    EXPECT_NE(registeredTypeExp1, registeredTypeExp2);
    EXPECT_NE(registeredTypeExp1, constructedTypeExp1);
    EXPECT_NE(registeredTypeExp1, constructedTypeExp2);
    EXPECT_NE(registeredTypeExp1, constructedTypeExp3);
    EXPECT_NE(registeredTypeExp1, constructedTypeExpValue1);
    EXPECT_NE(registeredTypeExp1, constructedTypeExpMixed1);

    EXPECT_NE(registeredTypeExp2, constructedTypeExp1);
    EXPECT_NE(registeredTypeExp2, constructedTypeExp2);
    EXPECT_NE(registeredTypeExp2, constructedTypeExp3);
    EXPECT_NE(registeredTypeExp2, constructedTypeExpValue1);
    EXPECT_NE(registeredTypeExp2, constructedTypeExpMixed1);

    EXPECT_NE(constructedTypeExp1, constructedTypeExp2);
    EXPECT_NE(constructedTypeExp1, constructedTypeExp3);
    EXPECT_NE(constructedTypeExp1, constructedTypeExpValue1);
    EXPECT_NE(constructedTypeExp1, constructedTypeExpMixed1);

    EXPECT_NE(constructedTypeExp2, constructedTypeExp3);
    EXPECT_NE(constructedTypeExp2, constructedTypeExpValue1);
    EXPECT_NE(constructedTypeExp2, constructedTypeExpMixed1);

    EXPECT_NE(constructedTypeExpValue1, constructedTypeExpMixed1);
}

TEST(TypeExpTest, resolve) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const auto testType = typeSystem.addAndGetType<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeExp typeExp(testUtils::TestType::getThisIdentifier());

    EXPECT_EQ(testType.get(), typeExp.resolve(typeSystem).get());
    babelwires::TypeExp constructedTypeExp(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                           testUtils::TestType::getThisIdentifier());
    babelwires::TypePtr newType = constructedTypeExp.resolve(typeSystem);
    EXPECT_EQ(newType->getTypeExp(), constructedTypeExp);
    EXPECT_EQ(constructedTypeExp.resolve(typeSystem).get(), constructedTypeExp.resolve(typeSystem).get());
}

TEST(TypeExpTest, tryResolveSuccess) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const auto testType = typeSystem.addAndGetType<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeExp typeExp(testUtils::TestType::getThisIdentifier());
    EXPECT_EQ(testType.get(), typeExp.tryResolve(typeSystem).get());

    babelwires::TypeExp constructedTypeExp(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                           testUtils::TestType::getThisIdentifier());
    babelwires::TypePtr newType = constructedTypeExp.tryResolve(typeSystem);
    EXPECT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeExp(), constructedTypeExp);
    EXPECT_EQ(constructedTypeExp.tryResolve(typeSystem).get(), constructedTypeExp.tryResolve(typeSystem).get());
}

// std::execution::par not currently supported on MacOs.
#ifndef __APPLE__

// Test resolving a number of TypeExps in parallel, where the typeExps are combinations of two type constructors.
// This is intended to test for thread-safety issues in the TypeExp resolution and the caches held by TypeConstructors.
TEST(TypeExpTest, tryResolveParallel) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    typeSystem.addType<testUtils::TestType>();
    typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    typeSystem.addTypeConstructor<testUtils::TestBinaryTypeConstructor>();

    struct Entry {
        babelwires::TypeExp m_typeExp;
        babelwires::TypePtr m_typePtr;
    };

    std::vector<Entry> vectorOfResolutions;
    vectorOfResolutions.emplace_back(Entry{testUtils::TestType::getThisIdentifier()});

    // The number of entries at a given depth is (2^(2^d)) - 1
    const auto getNumEntriesAtDepth = [](unsigned int d) { return (1 << (1 << d)) - 1; };
    const unsigned int depth = 4;
    // Every combination at depth 4 exceeds reasonable test time on slow machines, so cap it.
    const unsigned int maximumNumEntries = 16639;

    vectorOfResolutions.reserve(getNumEntriesAtDepth(depth));

    for (unsigned int d = 0; d < depth; ++d) {
        unsigned int currentSize = getNumEntriesAtDepth(d);
        for (unsigned int j = 0; j < currentSize; ++j) {
            if (vectorOfResolutions.size() >= maximumNumEntries) {
                break;
            }
            vectorOfResolutions.emplace_back(
                Entry{babelwires::TypeExp(testUtils::TestUnaryTypeConstructor::getThisIdentifier(), vectorOfResolutions[j].m_typeExp)});
            for (unsigned int k = 0; k < currentSize; ++k) {
                vectorOfResolutions.emplace_back(
                    Entry{babelwires::TypeExp(testUtils::TestBinaryTypeConstructor::getThisIdentifier(),
                                              vectorOfResolutions[j].m_typeExp, vectorOfResolutions[k].m_typeExp)});
            }
        }
    }

    ASSERT_EQ(vectorOfResolutions.size(), maximumNumEntries);

    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(vectorOfResolutions.begin(), vectorOfResolutions.end(), g);
    }

    // Resolve the TypeExps in parallel.
    std::for_each(
        std::execution::par,
        vectorOfResolutions.begin(), vectorOfResolutions.end(),
        [&typeSystem](auto& entry) { entry.m_typePtr = entry.m_typeExp.tryResolve(typeSystem); });

    for (const auto& entry : vectorOfResolutions) {
        EXPECT_NE(entry.m_typePtr, nullptr);
        EXPECT_EQ(entry.m_typePtr->getTypeExp(), entry.m_typeExp);
    }
}
#endif

TEST(TypeExpTest, tryResolveMixed) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;

    const auto testType = typeSystem.addAndGetType<testUtils::TestType>();
    const testUtils::TestUnaryTypeConstructor* unaryConstructor =
        typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    const testUtils::TestMixedTypeConstructor* mixedConstructor =
        typeSystem.addTypeConstructor<testUtils::TestMixedTypeConstructor>();

    babelwires::TypeExp constructedTestTypeExp(
        testUtils::TestMixedTypeConstructor::getThisIdentifier(),
        {{babelwires::TypeExp(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                              testUtils::TestType::getThisIdentifier())},
         {babelwires::StringValue(" is this string")}});

    babelwires::TypePtr constructedTestType = constructedTestTypeExp.resolve(typeSystem);
    const testUtils::TestType* const newTestType = constructedTestType->tryAs<testUtils::TestType>();
    ASSERT_NE(newTestType, nullptr);
    EXPECT_EQ(newTestType->m_defaultValue, "Default value is this string");
}

TEST(TypeExpTest, toStringSuccessTypes) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    EXPECT_EQ(babelwires::TypeExp().toString(), "[]");

    babelwires::RegisteredTypeId foo = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Foo", "Foofoo", "00000000-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    EXPECT_EQ(babelwires::TypeExp(foo).toString(), "Foofoo");

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "UNARY[[{0}]]", "11111111-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(unary0, foo).toString(), "UNARY[Foofoo]");

    // Too many arguments is allowed.
    EXPECT_EQ(babelwires::TypeExp(unary0, foo, foo).toString(), "UNARY[Foofoo]");

    EXPECT_EQ(babelwires::TypeExp(unary0, babelwires::TypeExp(unary0, foo)).toString(), "UNARY[UNARY[Foofoo]]");

    babelwires::TypeConstructorId unary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary1", "{0}++", "22222222-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(unary1, foo).toString(), "Foofoo++");
    EXPECT_EQ(babelwires::TypeExp(unary1, babelwires::TypeExp(unary0, foo)).toString(), "UNARY[Foofoo]++");
    EXPECT_EQ(babelwires::TypeExp(unary0, babelwires::TypeExp(unary1, foo)).toString(), "UNARY[Foofoo++]");

    babelwires::TypeConstructorId binary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary0", "{0} + {1}", "33333333-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(binary0, foo, babelwires::TypeExp(unary0, foo)).toString(), "Foofoo + UNARY[Foofoo]");
    EXPECT_EQ(babelwires::TypeExp(binary0, babelwires::TypeExp(unary0, foo), foo).toString(), "UNARY[Foofoo] + Foofoo");
    EXPECT_EQ(babelwires::TypeExp(unary0, babelwires::TypeExp(binary0, foo, foo)).toString(), "UNARY[Foofoo + Foofoo]");

    // With some escaped brackets.
    babelwires::TypeConstructorId binary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary1", "}}{1}{{}}{0}[[", "44444444-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(binary1, foo, babelwires::TypeExp(unary0, foo)).toString(),
              "}UNARY[Foofoo]{}Foofoo[");
    EXPECT_EQ(babelwires::TypeExp(binary1, babelwires::TypeExp(unary0, foo), foo).toString(),
              "}Foofoo{}UNARY[Foofoo][");
    EXPECT_EQ(babelwires::TypeExp(unary0, babelwires::TypeExp(binary1, foo, foo)).toString(),
              "UNARY[}Foofoo{}Foofoo[]");

    babelwires::TypeConstructorId withOptional = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "TC<{0?none}>([0])", "971f582f-c6f9-4e46-9499-20f5d1d44cfd",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(withOptional, babelwires::TypeConstructorArguments{{foo}, {babelwires::IntValue(12)}})
                  .toString(),
              "TC<Foofoo>(12)");
    EXPECT_EQ(babelwires::TypeExp(withOptional, babelwires::IntValue(12)).toString(), "TC<none>(12)");

    babelwires::TypeConstructorId withOp0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "{0|+}", "eebf69eb-bc91-44ce-8af6-8218f3d16705",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(withOp0, foo).toString(), "Foofoo");
    EXPECT_EQ(babelwires::TypeExp(withOp0, {{foo, foo}, {}}).toString(), "Foofoo+Foofoo");
    EXPECT_EQ(babelwires::TypeExp(withOp0, {{foo, foo, foo}, {}}).toString(), "Foofoo+Foofoo+Foofoo");

    babelwires::TypeConstructorId withOp1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "{0|<\\}>}", "7ce27b74-ba49-4c40-8c6a-c9b4cf09a005",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(withOp1, foo).toString(), "Foofoo");
    EXPECT_EQ(babelwires::TypeExp(withOp1, {{foo, foo}, {}}).toString(), "Foofoo<}>Foofoo");
    EXPECT_EQ(babelwires::TypeExp(withOp1, {{foo, foo, foo}, {}}).toString(), "Foofoo<}>Foofoo<}>Foofoo");
}

TEST(TypeExpTest, toStringSuccessValues) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::StringValue foo("foo");
    babelwires::StringValue bar("bar");

    babelwires::TypeConstructorId unary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary0", "UNARY[[[0]]]", "8b3068ee-c604-4fa1-895f-c85dffeceda4",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(unary0, foo.clone()).toString(), "UNARY[foo]");

    // Too many arguments is allowed.
    EXPECT_EQ(babelwires::TypeExp(unary0, foo.clone(), foo.clone()).toString(), "UNARY[foo]");

    babelwires::TypeConstructorId binary0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary0", "[0] + [1]", "48aaeb2e-6baf-427a-abf6-490e17da0978",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(binary0, foo.clone(), bar.clone()).toString(), "foo + bar");

    // With some escaped brackets.
    babelwires::TypeConstructorId binary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Binary1", "}}[1]{{]][0]{{", "51aa321f-6066-40c1-ad5e-a1d6c2f4fd00",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(binary1, foo.clone(), bar.clone()).toString(), "}bar{]foo{");

    babelwires::TypeConstructorId withOptional = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "TC([0?])", "971f582f-c6f9-4e46-9499-20f5d1d44cfd",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(withOptional, babelwires::TypeConstructorArguments{}).toString(), "TC()");
    EXPECT_EQ(babelwires::TypeExp(withOptional, babelwires::IntValue(12)).toString(), "TC(12)");

    babelwires::TypeConstructorId withOp0 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "([0|,])", "896144d4-9972-4162-81c8-336869bb9cfc",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(withOp0, foo.clone()).toString(), "(foo)");
    EXPECT_EQ(babelwires::TypeExp(withOp0, {{}, {foo.clone(), bar.clone()}}).toString(), "(foo,bar)");
    EXPECT_EQ(babelwires::TypeExp(withOp0, {{}, {foo.clone(), bar.clone(), foo.clone()}}).toString(), "(foo,bar,foo)");

    babelwires::TypeConstructorId withOp1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "WithOp", "[0|<\\]>]", "393cda51-e0a1-4610-85e6-2dbffa971225",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(withOp1, foo.clone()).toString(), "foo");
    EXPECT_EQ(babelwires::TypeExp(withOp1, {{}, {foo.clone(), bar.clone()}}).toString(), "foo<]>bar");
    EXPECT_EQ(babelwires::TypeExp(withOp1, {{}, {foo.clone(), bar.clone(), foo.clone()}}).toString(),
              "foo<]>bar<]>foo");
}

TEST(TypeExpTest, toStringSuccessMixed) {
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
    EXPECT_EQ(babelwires::TypeExp(mixed, {{foo, foo}, {bar.clone(), boo.clone(), bar.clone()}}).toString(),
              "Foofoo+bar<]>boo<]>bar+Foofoo");
}

TEST(TypeExpTest, toStringMalformed) {
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
    EXPECT_EQ(babelwires::TypeExp(unary0, foo).toString(), "MalformedTypeExp");

    babelwires::TypeConstructorId unary1 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary1", "oo{", "22222222-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(unary1, foo).toString(), "MalformedTypeExp");

    // This type of format string is not supported.
    babelwires::TypeConstructorId unary2 = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "Unary2", "oo{}pp", "33333333-2222-3333-4444-555566667777",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(unary2, foo).toString(), "MalformedTypeExp");

    // This type of format string is not supported.
    babelwires::TypeConstructorId valueBroken = babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
        "ValueBroken", "oo[pp", "18d028cb-de11-4974-86bf-068be2a509fd",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(babelwires::TypeExp(valueBroken, foo).toString(), "MalformedTypeExp");
}

TEST(TypeExpTest, serialization) {
    babelwires::TypeExp nullTypeExp;
    babelwires::TypeExp registeredTypeExp1(babelwires::RegisteredTypeId("Foo"));
    babelwires::TypeExp registeredTypeExp2(babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeExp constructedTypeExp1(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeExp constructedTypeExp2(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
                                            babelwires::RegisteredTypeId("Flerm"));
    babelwires::TypeExp constructedTypeExp3(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeExp(babelwires::TypeConstructorId("Flerm"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeExp constructedTypeExpValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeExp constructedTypeExpMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::RegisteredTypeId("Bar")}, {babelwires::IntValue(16)}});

    std::vector<babelwires::TypeExp> testRefs = {
        nullTypeExp,         registeredTypeExp1,  registeredTypeExp2,       constructedTypeExp1,
        constructedTypeExp2, constructedTypeExp3, constructedTypeExpValue1, constructedTypeExpMixed1};

    for (auto typeExp : testRefs) {
        std::string serializedContents;
        {
            babelwires::XmlSerializer serializer;
            serializer.serializeObject(typeExp);
            std::ostringstream os;
            serializer.write(os);
            serializedContents = std::move(os.str());
        }
        testUtils::TestLog log;
        babelwires::AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(deserializationReg, log);
        ASSERT_TRUE(deserializer.parse(serializedContents));
        auto typeExpPtrResult = deserializer.deserializeObject<babelwires::TypeExp>();
        ASSERT_TRUE(typeExpPtrResult);
        auto typeExpPtr = std::move(*typeExpPtrResult);
        deserializer.finalize();

        ASSERT_NE(typeExpPtr, nullptr);
        EXPECT_EQ(*typeExpPtr, typeExp);
    }
}

TEST(TypeExpTest, visitIdentifiers) {
    babelwires::TypeExp typeExp(testUtils::getTestRegisteredMediumIdentifier("Foo", 2),
                                testUtils::getTestRegisteredMediumIdentifier("Bar", 4),
                                babelwires::TypeExp(testUtils::getTestRegisteredMediumIdentifier("Flerm", 1),
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

    typeExp.visitIdentifiers(visitor1);
    typeExp.visitIdentifiers(visitor2);

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

TEST(TypeExpTest, hash) {
    babelwires::TypeExp nullTypeExp;
    babelwires::TypeExp registeredTypeExp1(babelwires::RegisteredTypeId("Foo"));
    babelwires::TypeExp registeredTypeExp2(babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeExp constructedTypeExp1(babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"));
    babelwires::TypeExp constructedTypeExp2(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeExp(babelwires::TypeConstructorId("Flerm"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeExp constructedTypeExp3(
        babelwires::TypeConstructorId("Foo"), babelwires::RegisteredTypeId("Bar"),
        babelwires::TypeExp(babelwires::TypeConstructorId("Oom"), babelwires::RegisteredTypeId("Erm")));
    babelwires::TypeExp constructedTypeExpValue1(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Bar"));
    babelwires::TypeExp constructedTypeExpValue2(babelwires::TypeConstructorId("Foo"), babelwires::StringValue("Erm"));
    babelwires::TypeExp constructedTypeExpMixed1(
        babelwires::TypeConstructorId("Foo"),
        babelwires::TypeConstructorArguments{{babelwires::RegisteredTypeId("Bar")}, {babelwires::IntValue(16)}});

    std::hash<babelwires::TypeExp> hasher;

    // In theory, some of these could fail due to a hash collision.
    EXPECT_NE(hasher(nullTypeExp), hasher(registeredTypeExp1));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(registeredTypeExp2));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(constructedTypeExp1));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(constructedTypeExp2));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(constructedTypeExp3));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(constructedTypeExpValue1));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(constructedTypeExpValue2));
    EXPECT_NE(hasher(registeredTypeExp1), hasher(constructedTypeExpMixed1));
    EXPECT_NE(hasher(constructedTypeExp1), hasher(constructedTypeExp2));
    EXPECT_NE(hasher(constructedTypeExp1), hasher(constructedTypeExp3));
    EXPECT_NE(hasher(constructedTypeExp1), hasher(constructedTypeExpValue1));
    EXPECT_NE(hasher(constructedTypeExp1), hasher(constructedTypeExpValue2));
    EXPECT_NE(hasher(constructedTypeExp1), hasher(constructedTypeExpMixed1));
    EXPECT_NE(hasher(constructedTypeExp2), hasher(constructedTypeExp3));
    EXPECT_NE(hasher(constructedTypeExp2), hasher(constructedTypeExp3));
    EXPECT_NE(hasher(constructedTypeExp2), hasher(constructedTypeExpValue1));
    EXPECT_NE(hasher(constructedTypeExp2), hasher(constructedTypeExpValue2));
    EXPECT_NE(hasher(constructedTypeExp2), hasher(constructedTypeExpMixed1));
    EXPECT_NE(hasher(constructedTypeExpValue1), hasher(constructedTypeExpValue2));
    EXPECT_NE(hasher(constructedTypeExpValue1), hasher(constructedTypeExpMixed1));
    EXPECT_NE(hasher(constructedTypeExpValue2), hasher(constructedTypeExpMixed1));
}