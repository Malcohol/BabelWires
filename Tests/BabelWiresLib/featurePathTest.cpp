#include <gtest/gtest.h>

#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"
#include "Tests/TestUtils/testLog.hpp"

#include "BabelWiresLib/Features/Path/featurePath.hpp"
#include "BabelWiresLib/Identifiers/declareIdentifier.hpp"
#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Features/arrayFeature.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"
#include "BabelWiresLib/Features/recordFeature.hpp"

#include "Common/Log/unifiedLog.hpp"
#include "Common/Serialization/XML/xmlDeserializer.hpp"
#include "Common/Serialization/XML/xmlSerializer.hpp"
#include "Common/exceptions.hpp"

TEST(FeaturePathTest, fieldIdentifiers) {
    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);
    hello.setDiscriminator(17);

    std::string helloStr = "Hello";
    babelwires::Identifier hello1(helloStr);
    hello1.setDiscriminator(27);

    // The "code" does not include the discriminator.
    EXPECT_EQ(hello.getDataAsCode(), hello1.getDataAsCode());
    EXPECT_LE(hello.getDataAsCode(), hello1.getDataAsCode());
    EXPECT_FALSE(hello.getDataAsCode() < hello1.getDataAsCode());

    // Discriminators are not used to distinguish fields.
    EXPECT_EQ(hello, hello1);
    std::hash<babelwires::Identifier> fieldHasher;
    EXPECT_EQ(fieldHasher(hello), fieldHasher(hello1));

    babelwires::Identifier goodbye("Byebye");
    goodbye.setDiscriminator(17);
    EXPECT_NE(hello, goodbye);

    // Only copy over unset discriminators. Reason:
    // If a field in a file has no discriminator, it will be updated by the project after opening.
    // However, if a field in a file already carries discrimintors, which don't resolve via their UUID,
    // we assume that they should be respected and not modified, even if they match fields.

    hello.copyDiscriminatorTo(hello1);
    EXPECT_EQ(hello1.getDiscriminator(), 27);

    hello1.setDiscriminator(0);
    hello.copyDiscriminatorTo(hello1);
    EXPECT_EQ(hello1.getDiscriminator(), hello.getDiscriminator());
}

// For sanity's sake, the identifiers are ordered alphabetically.
TEST(FeaturePathTest, fieldIdentifiersOrder) {
    babelwires::Identifier zero("A000");
    babelwires::Identifier ten("A10");
    babelwires::Identifier ant("ant");
    babelwires::Identifier antelope("antlpe");
    babelwires::Identifier Emu("Emu");
    babelwires::Identifier emu("emu");
    babelwires::Identifier Ibex("Ibex");
    babelwires::Identifier zebra("zebra");

    EXPECT_LT(zero, ten);
    EXPECT_LT(ten, Emu);
    EXPECT_LT(Emu, Ibex);
    EXPECT_LT(Ibex, ant);
    EXPECT_LT(ant, antelope);
    EXPECT_LT(antelope, emu);
    EXPECT_LT(emu, zebra);
}

TEST(FeaturePathTest, fieldIdentifierStringOutput) {
    babelwires::Identifier hello("Hello");
    {
        std::ostringstream os;
        os << hello;
        EXPECT_EQ(os.str(), "Hello");
    }
    {
        hello.setDiscriminator(15);
        std::ostringstream os;
        os << hello;
        EXPECT_EQ(os.str(), "Hello`15");
    }
}

TEST(FeaturePathTest, fieldIdentifierSerialization) {
    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.serializeToString(), "Hello");

    hello.setDiscriminator(81);
    EXPECT_EQ(hello.serializeToString(), "Hello`81");
}

TEST(FeaturePathTest, fieldIdentifierDeserialization) {
    const babelwires::Identifier hello = babelwires::Identifier::deserializeFromString("Hello");
    EXPECT_EQ(hello, "Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    const babelwires::Identifier hello1 = babelwires::Identifier::deserializeFromString("Hello`12");
    EXPECT_EQ(hello1, "Hello");
    EXPECT_EQ(hello1.getDiscriminator(), 12);

    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("H"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("H`1"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("HE`11"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Hel`111"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Hell`111"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Hello`111"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Hell33"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("He(33)`10"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Hello`255"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Helloo`65500"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("."));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("^-.-^"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("(%[EE"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("^-.-^`3"));

    EXPECT_THROW(babelwires::Identifier::deserializeFromString(""), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("02"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("12"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("`12"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("2Hello"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("Hællo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("Hello'111"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("Helloooo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("Helloo`65535"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("Hell`100000"), babelwires::ParseException);
}

TEST(FeaturePathTest, featureNameRegistrySameNames) {
    testUtils::TestLog log;

    babelwires::FieldNameRegistry fieldNameRegistry;

    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::Identifier id = fieldNameRegistry.addFieldName(
        hello, "Hello World", uuid, babelwires::FieldNameRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(fieldNameRegistry.getFieldName(id), "Hello World");

    babelwires::Identifier hello2("Hello");
    babelwires::Uuid uuid2("00000000-1111-2222-3333-000000000002");

    const babelwires::Identifier id2 = fieldNameRegistry.addFieldName(
        hello2, "Hello World 2", uuid2, babelwires::FieldNameRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id2.getDiscriminator(), 0);
    EXPECT_NE(id2.getDiscriminator(), id.getDiscriminator());
    EXPECT_EQ(fieldNameRegistry.getFieldName(id2), "Hello World 2");
}

TEST(FeaturePathTest, featureNameRegistryAuthoritativeFirst) {
    testUtils::TestLog log;

    babelwires::FieldNameRegistry fieldNameRegistry;

    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::Identifier id = fieldNameRegistry.addFieldName(
        hello, "Hello World", uuid, babelwires::FieldNameRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(fieldNameRegistry.getFieldName(id), "Hello World");

    // A provisional name updates to match an authoritative one.
    babelwires::Identifier hello1("Hello");
    EXPECT_EQ(hello, hello1);
    const babelwires::Identifier id1 = fieldNameRegistry.addFieldName(
        hello1, "Hello World 1", uuid, babelwires::FieldNameRegistry::Authority::isProvisional);
    EXPECT_EQ(id1, hello1);
    EXPECT_EQ(id1.getDiscriminator(), id.getDiscriminator());
    EXPECT_EQ(fieldNameRegistry.getFieldName(id1), "Hello World");
}

TEST(FeaturePathTest, featureNameRegistryProvisionalFirst) {
    testUtils::TestLog log;

    babelwires::FieldNameRegistry fieldNameRegistry;

    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::Identifier id = fieldNameRegistry.addFieldName(
        hello, "Hello World", uuid, babelwires::FieldNameRegistry::Authority::isProvisional);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(fieldNameRegistry.getFieldName(id), "Hello World");

    // A provisional name updates to match an authorative one.
    babelwires::Identifier hello1("Hello");
    EXPECT_EQ(hello, hello1);
    const babelwires::Identifier id1 = fieldNameRegistry.addFieldName(
        hello1, "Hello World 2", uuid, babelwires::FieldNameRegistry::Authority::isAuthoritative);
    EXPECT_EQ(id1, hello1);
    EXPECT_EQ(id1.getDiscriminator(), id.getDiscriminator());

    // The original ID will now obtain the new _authoritative_ name.
    EXPECT_EQ(fieldNameRegistry.getFieldName(id), "Hello World 2");
}

TEST(FeaturePathTest, featureNameRegistrySerializationDeserialization) {
    testUtils::TestLog log;
    std::string serializedContents;

    babelwires::Identifier id0("hello");
    babelwires::Identifier id1("byebye");
    const std::string name0 = "Name 0";
    const std::string name1 = "Name 1";
    {
        babelwires::FieldNameRegistry fieldNameRegistry;

        const babelwires::Uuid uuid0("00000000-1111-2222-3333-000000000001");
        const babelwires::Uuid uuid1("00000000-1111-2222-3333-000000000002");

        id0 = fieldNameRegistry.addFieldName(id0, name0, uuid0,
                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        id1 =
            fieldNameRegistry.addFieldName(id1, name1, uuid1, babelwires::FieldNameRegistry::Authority::isProvisional);

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(fieldNameRegistry);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        babelwires::AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto fieldNameRegPtr = deserializer.deserializeObject<babelwires::FieldNameRegistry>();
        ASSERT_NE(fieldNameRegPtr, nullptr);
        deserializer.finalize();

        EXPECT_EQ(fieldNameRegPtr->getFieldName(id0), name0);
        EXPECT_EQ(fieldNameRegPtr->getFieldName(id1), name1);
    }
}

TEST(FeaturePathTest, implicitFieldNameRegistration) {
    testUtils::TestLog log;

    // Work with a temporary global registry.
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    int discriminator = 0;
    for (int i = 0; i < 3; ++i) {
        // Repeating this line of code should be a NOOP.
        babelwires::Identifier hello = FIELD_NAME("hello", "Hello world", "00000000-1111-2222-3333-000000000001");
        EXPECT_EQ(hello, "hello");
        EXPECT_NE(hello.getDiscriminator(), 0);
        if (discriminator != 0) {
            EXPECT_EQ(hello.getDiscriminator(), discriminator);
        }
        discriminator = hello.getDiscriminator();

        EXPECT_EQ(babelwires::FieldNameRegistry::read()->getFieldName(hello), "Hello world");
    }
}

TEST(FeaturePathTest, implicitFieldNameRegistrationVector) {
    testUtils::TestLog log;

    // Work with a temporary global registry.
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    const babelwires::FieldIdentifiersSource source = {
        {"hello", "Hello world", "00000000-1111-2222-3333-000000000001"},
        {"byebye", "Goodbye world", "00000000-1111-2222-3333-000000000002"}};

    int hello_discriminator = 0;
    int goodbye_discriminator = 0;
    for (int i = 0; i < 3; ++i) {
        // Repeating this line of code should be a NOOP.
        babelwires::RegisteredFieldIdentifiers ids = FIELD_NAME_VECTOR(source);
        EXPECT_EQ(ids.size(), 2);

        babelwires::Identifier hello = ids[0];
        EXPECT_EQ(hello, "hello");
        EXPECT_NE(hello.getDiscriminator(), 0);
        if (hello_discriminator != 0) {
            EXPECT_EQ(hello.getDiscriminator(), hello_discriminator);
        }
        hello_discriminator = hello.getDiscriminator();
        EXPECT_EQ(babelwires::FieldNameRegistry::read()->getFieldName(hello), "Hello world");

        babelwires::Identifier goodbye = ids[1];
        EXPECT_EQ(goodbye, "byebye");
        EXPECT_NE(goodbye.getDiscriminator(), 0);
        if (goodbye_discriminator != 0) {
            EXPECT_EQ(goodbye.getDiscriminator(), goodbye_discriminator);
        }
        goodbye_discriminator = goodbye.getDiscriminator();
        EXPECT_EQ(babelwires::FieldNameRegistry::read()->getFieldName(goodbye), "Goodbye world");
    }
}

TEST(FeaturePathTest, pathStepOps) {
    babelwires::Identifier hello("Hello");
    babelwires::Identifier hello1("Hello");
    babelwires::Identifier goodbye("Byebye");

    babelwires::PathStep helloStep(hello);
    babelwires::PathStep hello1Step(hello1);
    babelwires::PathStep goodbyeStep(goodbye);

    EXPECT_TRUE(helloStep.isField());
    EXPECT_EQ(helloStep.getField(), hello);

    EXPECT_EQ(helloStep, hello1Step);
    EXPECT_NE(helloStep, goodbyeStep);
    EXPECT_LE(helloStep, hello1Step);
    EXPECT_LE(goodbyeStep, helloStep);
    EXPECT_LT(goodbyeStep, helloStep);

    babelwires::PathStep zeroStep(0);
    babelwires::PathStep sevenStep(7);

    EXPECT_TRUE(sevenStep.isIndex());
    EXPECT_EQ(sevenStep.getIndex(), 7);

    EXPECT_NE(zeroStep, sevenStep);
    EXPECT_LT(zeroStep, sevenStep);
    EXPECT_LE(zeroStep, sevenStep);

    EXPECT_NE(zeroStep, helloStep);
    EXPECT_LT(zeroStep, goodbyeStep);
}

TEST(FeaturePathTest, pathStepDiscriminator) {
    babelwires::PathStep helloStep(babelwires::Identifier("Hello"));

    helloStep.asField()->setDiscriminator(12);
    EXPECT_EQ(helloStep.asField()->getDiscriminator(), 12);

    helloStep.getField().setDiscriminator(18);
    EXPECT_EQ(helloStep.getField().getDiscriminator(), 18);
}

TEST(FeaturePathTest, pathStepSerialization) {
    babelwires::PathStep hello("Hello");
    EXPECT_EQ(hello.serializeToString(), "Hello");

    babelwires::PathStep hello2("Hello");
    hello2.getField().setDiscriminator(199);
    EXPECT_EQ(hello2.serializeToString(), "Hello`199");

    babelwires::PathStep index(10);
    EXPECT_EQ(index.serializeToString(), "10");
}

TEST(FeaturePathTest, pathStepDeserialization) {
    babelwires::PathStep step(0);
    EXPECT_NO_THROW(step = babelwires::PathStep::deserializeFromString("Hello"));
    EXPECT_TRUE(step.isField());
    EXPECT_EQ(step.getField(), babelwires::Identifier("Hello"));
    EXPECT_EQ(step.getField().getDiscriminator(), 0);

    babelwires::PathStep step1(0);
    EXPECT_NO_THROW(step1 = babelwires::PathStep::deserializeFromString("Hello`2"));
    EXPECT_TRUE(step1.isField());
    EXPECT_EQ(step1.getField(), babelwires::Identifier("Hello"));
    EXPECT_EQ(step1.getField().getDiscriminator(), 2);

    babelwires::PathStep step2("Erm");
    EXPECT_NO_THROW(step2 = babelwires::PathStep::deserializeFromString("10"));
    EXPECT_TRUE(step2.isIndex());
    EXPECT_EQ(step2.getIndex(), 10);

    EXPECT_THROW(babelwires::PathStep::deserializeFromString("`"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("/"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("Hællo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("Hello`65536"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("1`23"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("3Hello"), babelwires::ParseException);
}

TEST(FeaturePathTest, pathOps) {
    babelwires::FeaturePath path1;

    path1.pushStep(babelwires::PathStep("Forb"));
    path1.pushStep(babelwires::PathStep("Erm"));
    path1.pushStep(babelwires::PathStep(12));
    EXPECT_EQ(path1.getLastStep(), path1.getStep(2));

    babelwires::FeaturePath path2 = path1;
    EXPECT_EQ(path1, path2);
    EXPECT_LE(path1, path2);
    EXPECT_TRUE(path1.isPrefixOf(path2));
    EXPECT_FALSE(path1.isStrictPrefixOf(path2));

    EXPECT_EQ(path1.getNumSteps(), 3);
    path1.popStep();
    EXPECT_EQ(path1.getNumSteps(), 2);
    EXPECT_NE(path1, path2);
    EXPECT_LT(path1, path2);
    EXPECT_TRUE(path1.isPrefixOf(path2));
    EXPECT_TRUE(path1.isStrictPrefixOf(path2));
    path2.truncate(2);
    EXPECT_EQ(path1, path2);

    path1.popStep();
    EXPECT_EQ(path1.getNumSteps(), 1);
    path1.popStep();
    EXPECT_EQ(path1.getNumSteps(), 0);

    path2.truncate(0);
    EXPECT_EQ(path2.getNumSteps(), 0);
}

TEST(FeaturePathTest, pathIteration) {
    babelwires::FeaturePath path1;

    path1.pushStep(babelwires::PathStep("Forb"));
    path1.pushStep(babelwires::PathStep("Erm"));
    path1.pushStep(babelwires::PathStep(12));

    babelwires::FeaturePath path2;
    for (const auto& s : path1) {
        path2.pushStep(s);
    }
    EXPECT_EQ(path2.getNumSteps(), 3);
    EXPECT_EQ(path2, path1);
}

TEST(FeaturePathTest, pathFollow) {
    testUtils::TestLog log;
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    libTestUtils::TestRecordFeature testRecordFeature;

    // Now set up some paths.
    babelwires::FeaturePath pathToRecord(&testRecordFeature);
    babelwires::FeaturePath pathToInt(testRecordFeature.m_intFeature);
    babelwires::FeaturePath pathToArray(testRecordFeature.m_arrayFeature);
    babelwires::FeaturePath pathToElem0(testRecordFeature.m_elem0);
    babelwires::FeaturePath pathToElem1(testRecordFeature.m_elem1);
    babelwires::FeaturePath pathToSubRecord(testRecordFeature.m_subRecordFeature);
    babelwires::FeaturePath pathToInt2(testRecordFeature.m_intFeature2);

    EXPECT_EQ(&pathToRecord.follow(testRecordFeature), &testRecordFeature);
    EXPECT_EQ(&pathToInt.follow(testRecordFeature), testRecordFeature.m_intFeature);
    EXPECT_EQ(&pathToArray.follow(testRecordFeature), testRecordFeature.m_arrayFeature);
    EXPECT_EQ(&pathToElem0.follow(testRecordFeature), testRecordFeature.m_elem0);
    EXPECT_EQ(&pathToElem1.follow(testRecordFeature), testRecordFeature.m_elem1);
    EXPECT_EQ(&pathToSubRecord.follow(testRecordFeature), testRecordFeature.m_subRecordFeature);
    EXPECT_EQ(&pathToInt2.follow(testRecordFeature), testRecordFeature.m_intFeature2);

    EXPECT_EQ(pathToRecord.getNumSteps(), 0);
    EXPECT_EQ(pathToInt.getNumSteps(), 1);
    EXPECT_EQ(pathToArray.getNumSteps(), 1);
    EXPECT_EQ(pathToElem0.getNumSteps(), 2);
    EXPECT_EQ(pathToElem1.getNumSteps(), 2);
    EXPECT_EQ(pathToSubRecord.getNumSteps(), 1);
    EXPECT_EQ(pathToInt2.getNumSteps(), 2);

    EXPECT_EQ(pathToInt.getStep(0), babelwires::PathStep(testRecordFeature.m_intId));
    EXPECT_EQ(pathToArray.getStep(0), babelwires::PathStep(testRecordFeature.m_arrayId));
    EXPECT_EQ(pathToElem0.getStep(0), babelwires::PathStep(testRecordFeature.m_arrayId));
    EXPECT_EQ(pathToElem0.getStep(1), babelwires::PathStep(0));
    EXPECT_EQ(pathToElem1.getStep(0), babelwires::PathStep(testRecordFeature.m_arrayId));
    EXPECT_EQ(pathToElem1.getStep(1), babelwires::PathStep(1));
    EXPECT_EQ(pathToSubRecord.getStep(0), babelwires::PathStep(testRecordFeature.m_recordId));
    EXPECT_EQ(pathToInt2.getStep(0), babelwires::PathStep(testRecordFeature.m_recordId));
    EXPECT_EQ(pathToInt2.getStep(1), babelwires::PathStep(testRecordFeature.m_int2Id));
}

TEST(FeaturePathTest, pathResolve) {
    testUtils::TestLog log;
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    babelwires::FeaturePath pathToInt;
    babelwires::FeaturePath pathToArray;
    babelwires::FeaturePath pathToElem0;
    babelwires::FeaturePath pathToElem1;
    babelwires::FeaturePath pathToSubRecord;
    babelwires::FeaturePath pathToInt2;

    pathToInt.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_intIdInitializer));
    pathToArray.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_arrayIdInitializer));
    pathToElem0.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_arrayIdInitializer));
    pathToElem0.pushStep(babelwires::PathStep(0));
    pathToElem1.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_arrayIdInitializer));
    pathToElem1.pushStep(babelwires::PathStep(1));
    pathToSubRecord.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_recordIdInitializer));
    pathToInt2.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_recordIdInitializer));
    pathToInt2.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_int2IdInitializer));

    libTestUtils::TestRecordFeature testRecordFeature;

    EXPECT_EQ(pathToInt.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToArray.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToElem0.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToElem1.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToSubRecord.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToInt2.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToInt2.getStep(1).getField().getDiscriminator(), 0);

    // The paths should work even though they didn't use registered field identifiers.
    EXPECT_EQ(&pathToInt.follow(testRecordFeature), testRecordFeature.m_intFeature);
    EXPECT_EQ(&pathToArray.follow(testRecordFeature), testRecordFeature.m_arrayFeature);
    EXPECT_EQ(&pathToElem0.follow(testRecordFeature), testRecordFeature.m_elem0);
    EXPECT_EQ(&pathToElem1.follow(testRecordFeature), testRecordFeature.m_elem1);
    EXPECT_EQ(&pathToSubRecord.follow(testRecordFeature), testRecordFeature.m_subRecordFeature);
    EXPECT_EQ(&pathToInt2.follow(testRecordFeature), testRecordFeature.m_intFeature2);

    // The field identifiers in the paths should now be resolved.
    EXPECT_NE(pathToInt.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToArray.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToElem0.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToElem1.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToSubRecord.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToInt2.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToInt2.getStep(1).getField().getDiscriminator(), 0);

    // And they should be resolved to the values in the registered identifiers.
    EXPECT_EQ(pathToInt.getStep(0).getField().getDiscriminator(), testRecordFeature.m_intId.getDiscriminator());
    EXPECT_EQ(pathToArray.getStep(0).getField().getDiscriminator(), testRecordFeature.m_arrayId.getDiscriminator());
    EXPECT_EQ(pathToElem0.getStep(0).getField().getDiscriminator(), testRecordFeature.m_arrayId.getDiscriminator());
    EXPECT_EQ(pathToElem1.getStep(0).getField().getDiscriminator(), testRecordFeature.m_arrayId.getDiscriminator());
    EXPECT_EQ(pathToSubRecord.getStep(0).getField().getDiscriminator(),
              testRecordFeature.m_recordId.getDiscriminator());
    EXPECT_EQ(pathToInt2.getStep(0).getField().getDiscriminator(), testRecordFeature.m_recordId.getDiscriminator());
    EXPECT_EQ(pathToInt2.getStep(1).getField().getDiscriminator(), testRecordFeature.m_int2Id.getDiscriminator());
}

TEST(FeaturePathTest, pathTryFollow) {
    testUtils::TestLog log;
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    libTestUtils::TestRecordFeature testRecordFeature;

    // Now set up some paths.
    babelwires::FeaturePath pathToRecord(&testRecordFeature);
    babelwires::FeaturePath pathToInt(testRecordFeature.m_intFeature);
    babelwires::FeaturePath pathToArray(testRecordFeature.m_arrayFeature);
    babelwires::FeaturePath pathToElem0(testRecordFeature.m_elem0);
    babelwires::FeaturePath pathToElem1(testRecordFeature.m_elem1);
    babelwires::FeaturePath pathToSubRecord(testRecordFeature.m_subRecordFeature);
    babelwires::FeaturePath pathToInt2(testRecordFeature.m_intFeature2);

    EXPECT_EQ(pathToRecord.tryFollow(testRecordFeature), &testRecordFeature);
    EXPECT_EQ(pathToInt.tryFollow(testRecordFeature), testRecordFeature.m_intFeature);
    EXPECT_EQ(pathToArray.tryFollow(testRecordFeature), testRecordFeature.m_arrayFeature);
    EXPECT_EQ(pathToElem0.tryFollow(testRecordFeature), testRecordFeature.m_elem0);
    EXPECT_EQ(pathToElem1.tryFollow(testRecordFeature), testRecordFeature.m_elem1);
    EXPECT_EQ(pathToSubRecord.tryFollow(testRecordFeature), testRecordFeature.m_subRecordFeature);
    EXPECT_EQ(pathToInt2.tryFollow(testRecordFeature), testRecordFeature.m_intFeature2);
}

TEST(FeaturePathTest, pathFollowFail) {
    testUtils::TestLog log;
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    babelwires::FeaturePath pathToNonField;
    babelwires::FeaturePath pathToNonIndex;
    babelwires::FeaturePath pathOffEndOfArray;
    babelwires::FeaturePath pathValueAsRecord;
    babelwires::FeaturePath pathValueAsArray;

    pathToNonField.pushStep(babelwires::PathStep("Forb"));
    pathToNonIndex.pushStep(babelwires::PathStep(0));
    pathOffEndOfArray.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_arrayIdInitializer));
    pathOffEndOfArray.pushStep(babelwires::PathStep(5));
    pathValueAsRecord.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_int2IdInitializer));
    pathValueAsRecord.pushStep(babelwires::PathStep("flerm"));
    pathValueAsArray.pushStep(babelwires::PathStep(libTestUtils::TestRecordFeature::s_int2IdInitializer));
    pathValueAsArray.pushStep(babelwires::PathStep(12));

    libTestUtils::TestRecordFeature testRecordFeature;

    EXPECT_THROW(pathToNonField.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathToNonIndex.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathOffEndOfArray.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathValueAsRecord.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathValueAsArray.follow(testRecordFeature), babelwires::ModelException);

    EXPECT_EQ(pathToNonField.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathToNonIndex.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathOffEndOfArray.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathValueAsRecord.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathValueAsArray.tryFollow(testRecordFeature), nullptr);
}

TEST(FeaturePathTest, pathSerialization) {
    babelwires::FeaturePath path;

    EXPECT_EQ(path.serializeToString(), "");

    path.pushStep(babelwires::PathStep("Forb"));
    path.pushStep(babelwires::PathStep("Erm"));
    path.pushStep(babelwires::PathStep(12));

    EXPECT_EQ(path.serializeToString(), "Forb/Erm/12");

    path.getStep(0).getField().setDiscriminator(2);
    path.getStep(1).getField().setDiscriminator(4);

    EXPECT_EQ(path.serializeToString(), "Forb`2/Erm`4/12");
}

TEST(FeaturePathTest, pathDeserialization) {
    EXPECT_EQ(babelwires::FeaturePath::deserializeFromString("").getNumSteps(), 0);

    babelwires::FeaturePath path1;
    EXPECT_NO_THROW(path1 = babelwires::FeaturePath::deserializeFromString("Forb/12/Erm"));
    EXPECT_EQ(path1.getNumSteps(), 3);
    EXPECT_EQ(path1.getStep(0), babelwires::PathStep("Forb"));
    EXPECT_EQ(path1.getStep(1), babelwires::PathStep(12));
    EXPECT_EQ(path1.getStep(2), babelwires::PathStep("Erm"));

    babelwires::FeaturePath path2;
    EXPECT_NO_THROW(path2 = babelwires::FeaturePath::deserializeFromString("12/Forb/Erm"));
    EXPECT_EQ(path2.getNumSteps(), 3);
    EXPECT_EQ(path2.getStep(0), babelwires::PathStep(12));
    EXPECT_EQ(path2.getStep(1), babelwires::PathStep("Forb"));
    EXPECT_EQ(path2.getStep(2), babelwires::PathStep("Erm"));

    babelwires::FeaturePath path3;
    EXPECT_NO_THROW(path3 = babelwires::FeaturePath::deserializeFromString("Forb`2/12/Erm`4"));
    EXPECT_EQ(path3.getNumSteps(), 3);
    EXPECT_EQ(path3.getStep(0), babelwires::PathStep("Forb"));
    EXPECT_EQ(path3.getStep(0).getField().getDiscriminator(), 2);
    EXPECT_EQ(path3.getStep(1), babelwires::PathStep(12));
    EXPECT_EQ(path3.getStep(2), babelwires::PathStep("Erm"));
    EXPECT_EQ(path3.getStep(2).getField().getDiscriminator(), 4);

    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("Foo//Bar"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("/Foo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("`23/Foo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("Foo/Hællo/Foo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("Foo/3Hello"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("Foo/Erm`233232"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FeaturePath::deserializeFromString("Foo/Erm/"), babelwires::ParseException);
}

TEST(FeaturePathTest, pathHash) {
    babelwires::FeaturePath path;
    const std::size_t hashWhenEmpty = path.getHash();

    path.pushStep(0);
    const std::size_t hashWithZero = path.getHash();

    path.popStep();
    const std::size_t hashWhenEmptyAgain = path.getHash();

    path.pushStep(1);
    const std::size_t hashWithOne = path.getHash();

    path.pushStep(babelwires::PathStep("Hello"));
    const std::size_t hashWithOneHello = path.getHash();

    path.popStep();
    const std::size_t hashWithOneAgain = path.getHash();
    path.pushStep(babelwires::PathStep("Byebye"));
    const std::size_t hashWithOneByebye = path.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_EQ(hashWhenEmpty, hashWhenEmptyAgain);
    EXPECT_NE(hashWhenEmpty, hashWithZero);
    EXPECT_NE(hashWithZero, hashWithOne);
    EXPECT_NE(hashWithOne, hashWithOneHello);
    EXPECT_EQ(hashWithOne, hashWithOneAgain);
    EXPECT_NE(hashWithOneHello, hashWithOneByebye);
}
