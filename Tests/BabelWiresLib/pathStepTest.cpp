#include <gtest/gtest.h>

#include <BabelWiresLib/Path/pathStep.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/Log/unifiedLog.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>
#include <Common/exceptions.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(PathStepTest, projection) {
    babelwires::ShortId hello("Hello");
    babelwires::ShortId hello1("Hello");
    babelwires::ShortId goodbye("Byebye");

    babelwires::PathStep notAStep;
    EXPECT_TRUE(notAStep.isNotAStep());
    EXPECT_FALSE(notAStep.isField());
    EXPECT_FALSE(notAStep.isIndex());

    babelwires::PathStep helloStep(hello);
    babelwires::PathStep hello1Step(hello1);
    babelwires::PathStep goodbyeStep(goodbye);

    EXPECT_TRUE(helloStep.isField());
    EXPECT_EQ(helloStep.getField(), hello);
    EXPECT_NE(helloStep.asField(), nullptr);
    EXPECT_EQ(helloStep.asIndex(), nullptr);
    EXPECT_EQ(*helloStep.asField(), hello);

    babelwires::PathStep zeroStep(0);
    babelwires::PathStep sevenStep(7);

    EXPECT_TRUE(sevenStep.isIndex());
    EXPECT_EQ(sevenStep.getIndex(), 7);
    EXPECT_NE(sevenStep.asIndex(), nullptr);
    EXPECT_EQ(sevenStep.asField(), nullptr);
    EXPECT_EQ(*sevenStep.asIndex(), 7);
}

TEST(PathStepTest, comparison) {
    babelwires::ShortId hello("Hello");
    babelwires::ShortId hello1("Hello");
    babelwires::ShortId goodbye("Byebye");
    babelwires::ShortId hi("Hi");

    babelwires::PathStep helloStep(hello);
    babelwires::PathStep hello1Step(hello1);
    babelwires::PathStep goodbyeStep(goodbye);
    babelwires::PathStep hiStep(hi);

    EXPECT_EQ(helloStep, hello1Step);
    EXPECT_NE(helloStep, goodbyeStep);
    EXPECT_LE(helloStep, hello1Step);
    EXPECT_LE(goodbyeStep, helloStep);
    EXPECT_LT(goodbyeStep, helloStep);
    EXPECT_LT(helloStep, hiStep);

    babelwires::PathStep zeroStep(0);
    babelwires::PathStep sevenStep(7);
    // Confirm that an index with the same data representation as an identifier still doesn't match. 
    // Note: This test only works on little endian platforms.
    babelwires::PathStep hiIndexStep('H' * 256 + 'i');

    EXPECT_NE(zeroStep, sevenStep);
    EXPECT_LT(zeroStep, sevenStep);
    EXPECT_LE(zeroStep, sevenStep);

    EXPECT_NE(zeroStep, helloStep);
    EXPECT_LT(zeroStep, goodbyeStep);

    EXPECT_NE(hiStep, hiIndexStep);
}

TEST(PathStepTest, pathStepDiscriminator) {
    babelwires::PathStep helloStep(babelwires::ShortId("Hello"));

    helloStep.asField()->setDiscriminator(12);
    EXPECT_EQ(helloStep.asField()->getDiscriminator(), 12);

    helloStep.getField().setDiscriminator(18888);
    EXPECT_EQ(helloStep.getField().getDiscriminator(), 18888);
}

TEST(PathStepTest, serialization) {
    babelwires::PathStep hello("Hello");
    EXPECT_EQ(hello.serializeToString(), "Hello");

    babelwires::PathStep hello2("Hello");
    hello2.getField().setDiscriminator(1099);
    EXPECT_EQ(hello2.serializeToString(), "Hello'1099");

    babelwires::PathStep index(10);
    EXPECT_EQ(index.serializeToString(), "10");

    babelwires::PathStep notAStep;
    EXPECT_EQ(notAStep.serializeToString(), babelwires::PathStep::c_notAStepRepresentation);
}

TEST(PathStepTest, deserialization) {
    babelwires::PathStep step(0);
    EXPECT_NO_THROW(step = babelwires::PathStep::deserializeFromString("Hello"));
    EXPECT_TRUE(step.isField());
    EXPECT_EQ(step.getField(), babelwires::ShortId("Hello"));
    EXPECT_EQ(step.getField().getDiscriminator(), 0);

    babelwires::PathStep step1(0);
    EXPECT_NO_THROW(step1 = babelwires::PathStep::deserializeFromString("Hello'2"));
    EXPECT_TRUE(step1.isField());
    EXPECT_EQ(step1.getField(), babelwires::ShortId("Hello"));
    EXPECT_EQ(step1.getField().getDiscriminator(), 2);

    babelwires::PathStep step2("Erm");
    EXPECT_NO_THROW(step2 = babelwires::PathStep::deserializeFromString("10"));
    EXPECT_TRUE(step2.isIndex());
    EXPECT_EQ(step2.getIndex(), 10);

    babelwires::PathStep notAStep;
    EXPECT_NO_THROW(notAStep = babelwires::PathStep::deserializeFromString(babelwires::PathStep::c_notAStepRepresentation));
    EXPECT_TRUE(notAStep.isNotAStep());
    
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("'"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("/"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("Hællo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("Hello'65536"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("1'23"), babelwires::ParseException);
    EXPECT_THROW(babelwires::PathStep::deserializeFromString("3Hello"), babelwires::ParseException);
}

TEST(PathStepTest, stringRepresentations) {
    testUtils::TestLog log;
    babelwires::IdentifierRegistry reg;
    babelwires::ShortId hello0("Hi");
    reg.addShortIdWithMetadata(hello0, "Hello World 0", "eaae7f1c-5d5f-45c8-8526-e1a5642c766c", babelwires::IdentifierRegistry::Authority::isAuthoritative);
    babelwires::ShortId hello1("Hi");
    reg.addShortIdWithMetadata(hello1, "Hello World 1", "0d28eb5f-010b-480d-8c5d-2af765817252", babelwires::IdentifierRegistry::Authority::isAuthoritative);
    babelwires::ShortId hello2("Hi");
    hello2 = reg.addShortIdWithMetadata(hello2, "Hello World 2", "9e4d2819-f802-4068-8132-a15ceb126d23", babelwires::IdentifierRegistry::Authority::isAuthoritative);

    babelwires::PathStep helloStep(hello2);
    babelwires::PathStep index(10);
    babelwires::PathStep notAStep;

    {
        std::ostringstream os;
        os << helloStep;
        EXPECT_EQ(os.str(), "Hi");
    }
    {
        std::ostringstream os;
        os << index;
        EXPECT_EQ(os.str(), "10");
    }
    {
        std::ostringstream os;
        os << notAStep;
        EXPECT_EQ(os.str(), babelwires::PathStep::c_notAStepRepresentation);
    }

    {
        std::ostringstream os;
        helloStep.writeToStreamReadable(os, reg);
        EXPECT_EQ(os.str(), "Hello World 2");
    }
    {
        std::ostringstream os;
        index.writeToStreamReadable(os, reg);
        EXPECT_EQ(os.str(), "[10]");
    }
    {
        std::ostringstream os;
        notAStep.writeToStreamReadable(os, reg);
        EXPECT_EQ(os.str(), babelwires::PathStep::c_notAStepRepresentation);
    }
}

