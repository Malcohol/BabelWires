#include <gtest/gtest.h>

#include <Tests/TestUtils/testLog.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Log/unifiedLog.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>
#include <Common/exceptions.hpp>

TEST(IdentifierTest, identifiers) {
    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);
    hello.setDiscriminator(17);

    std::string helloStr = "Hello";
    babelwires::Identifier hello1(helloStr);
    hello1.setDiscriminator(27);

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
TEST(IdentifierTest, identifierOrder) {
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

TEST(IdentifierTest, identifierStringOutput) {
    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.toString(), "Hello");
    {
        std::ostringstream os;
        os << hello;
        EXPECT_EQ(os.str(), "Hello");
    }
    {
        hello.setDiscriminator(15);
        std::ostringstream os;
        os << hello;
        EXPECT_EQ(os.str(), "Hello");
    }
}

TEST(IdentifierTest, identifierSerialization) {
    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.serializeToString(), "Hello");

    hello.setDiscriminator(81);
    EXPECT_EQ(hello.serializeToString(), "Hello`81");
}

TEST(IdentifierTest, identifierDeserialization) {
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
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("He_33_`10"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Hello`255"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("Helloo`65500"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("_"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("_o_o_"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("___EE"));
    EXPECT_NO_THROW(babelwires::Identifier::deserializeFromString("_o_o_`3"));

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
    EXPECT_THROW(babelwires::Identifier::deserializeFromString("^-.-^`3"), babelwires::ParseException);
}

TEST(IdentifierTest, longIdentifiers) {
    babelwires::LongIdentifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);
    hello.setDiscriminator(17);

    std::string helloStr = "Hello";
    babelwires::LongIdentifier hello1(helloStr);
    hello1.setDiscriminator(27);

    // Discriminators are not used to distinguish fields.
    EXPECT_EQ(hello, hello1);
    std::hash<babelwires::LongIdentifier> fieldHasher;
    EXPECT_EQ(fieldHasher(hello), fieldHasher(hello1));

    babelwires::LongIdentifier goodbye("AMuchLongerIndetifier");
    goodbye.setDiscriminator(17);
    EXPECT_NE(hello, goodbye);

    hello.copyDiscriminatorTo(hello1);
    EXPECT_EQ(hello1.getDiscriminator(), 27);

    hello1.setDiscriminator(0);
    hello.copyDiscriminatorTo(hello1);
    EXPECT_EQ(hello1.getDiscriminator(), hello.getDiscriminator());
}

TEST(IdentifierTest, longIdentifierOrder) {
    babelwires::LongIdentifier zero("A000");
    babelwires::LongIdentifier ten("A10");
    babelwires::LongIdentifier ant("ant");
    babelwires::LongIdentifier antelope("antelopes");
    babelwires::LongIdentifier Emu("Emu");
    babelwires::LongIdentifier emu("emu");
    babelwires::LongIdentifier Ibex("Ibex");
    babelwires::LongIdentifier zebra("zebra");

    EXPECT_LT(zero, ten);
    EXPECT_LT(ten, Emu);
    EXPECT_LT(Emu, Ibex);
    EXPECT_LT(Ibex, ant);
    EXPECT_LT(ant, antelope);
    EXPECT_LT(antelope, emu);
    EXPECT_LT(emu, zebra);

    babelwires::LongIdentifier hello0("aggggggggggggggggggggg");
    babelwires::LongIdentifier hello1("ggggggggggaggggggggggg");
    babelwires::LongIdentifier hello2("ggggggggggggggggggggga");
    babelwires::LongIdentifier hello3("gggggggggggggggggggggg");

    EXPECT_LT(hello0, hello1);
    EXPECT_LT(hello1, hello2);
    EXPECT_LT(hello2, hello3);
}

TEST(IdentifierTest, longIdentifierStringOutput) {
    babelwires::LongIdentifier hello("Hello_world");
    EXPECT_EQ(hello.toString(), "Hello_world");
    {
        std::ostringstream os;
        os << hello;
        EXPECT_EQ(os.str(), "Hello_world");
    }
    {
        hello.setDiscriminator(15);
        std::ostringstream os;
        os << hello;
        EXPECT_EQ(os.str(), "Hello_world");
    }
}

TEST(IdentifierTest, longIdentifierSerialization) {
    babelwires::LongIdentifier hello("Hello_world");
    EXPECT_EQ(hello.serializeToString(), "Hello_world");

    hello.setDiscriminator(81);
    EXPECT_EQ(hello.serializeToString(), "Hello_world`81");
}


TEST(IdentifierTest, longIdentifierDeserialization) {
    const babelwires::LongIdentifier hello = babelwires::LongIdentifier::deserializeFromString("Hello_world");
    EXPECT_EQ(hello, "Hello_world");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    const babelwires::LongIdentifier hello1 = babelwires::LongIdentifier::deserializeFromString("Hello_world`12");
    EXPECT_EQ(hello1, "Hello_world");
    EXPECT_EQ(hello1.getDiscriminator(), 12);

    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("H"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("H`1"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("HE`11"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Hel`111"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Hell`111"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Hello`111"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Hell33"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("He_33_`10"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Hello`255"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Helloo`65500"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("Hello_incredible_world`65500"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("_"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("_o_o_"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("___EE"));
    EXPECT_NO_THROW(babelwires::LongIdentifier::deserializeFromString("_o_o_`3"));

    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString(""), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("02"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("12"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("`12"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("2Hello"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("Hællo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("Hello_incredible_world_"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("Hello_incredible_world'111"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("Helloo`65535"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("Hell`100000"), babelwires::ParseException);
    EXPECT_THROW(babelwires::LongIdentifier::deserializeFromString("^-.-^`3"), babelwires::ParseException);
}

TEST(IdentifierTest, shortToLongIdentifiers) {
    const babelwires::Identifier hello = "Hello";
    const babelwires::LongIdentifier hello1 = hello;
    EXPECT_EQ(hello, hello1);

    hello.setDiscriminator(12);
    const babelwires::LongIdentifier hello2 = hello;
    EXPECT_EQ(hello, hello2);
    EXPECT_EQ(hello2.getDiscriminator(), 12);
}

TEST(IdentifierTest, longToShortIdentifiers) {
    const babelwires::LongIdentifier hello = "Helloo";
    const babelwires::Identifier hello1(hello);
    EXPECT_EQ(hello, hello1);

    hello.setDiscriminator(12);
    const babelwires::Identifier hello2(hello);
    EXPECT_EQ(hello, hello2);
    EXPECT_EQ(hello2.getDiscriminator(), 12);

    const babelwires::LongIdentifier longHello = "Hellooo";
    EXPECT_THROW(babelwires::Identifier shortHello(longHello), babelwires::ParseException);
}

TEST(IdentifierTest, identifierRegistrySameNames) {
    testUtils::TestLog log;

    babelwires::IdentifierRegistry identifierRegistry;

    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::Identifier id = identifierRegistry.addIdentifierWithMetadata(
        hello, "Hello World", uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(identifierRegistry.getName(id), "Hello World");

    babelwires::Identifier hello2("Hello");
    babelwires::Uuid uuid2("00000000-1111-2222-3333-000000000002");

    const babelwires::Identifier id2 = identifierRegistry.addIdentifierWithMetadata(
        hello2, "Hello World 2", uuid2, babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id2.getDiscriminator(), 0);
    EXPECT_NE(id2.getDiscriminator(), id.getDiscriminator());
    EXPECT_EQ(identifierRegistry.getName(id2), "Hello World 2");
}

TEST(IdentifierTest, identifierRegistryAuthoritativeFirst) {
    testUtils::TestLog log;

    babelwires::IdentifierRegistry identifierRegistry;

    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::Identifier id = identifierRegistry.addIdentifierWithMetadata(
        hello, "Hello World", uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(identifierRegistry.getName(id), "Hello World");

    // A provisional name updates to match an authoritative one.
    babelwires::Identifier hello1("Hello");
    EXPECT_EQ(hello, hello1);
    const babelwires::Identifier id1 = identifierRegistry.addIdentifierWithMetadata(
        hello1, "Hello World 1", uuid, babelwires::IdentifierRegistry::Authority::isProvisional);
    EXPECT_EQ(id1, hello1);
    EXPECT_EQ(id1.getDiscriminator(), id.getDiscriminator());
    EXPECT_EQ(identifierRegistry.getName(id1), "Hello World");
}

TEST(IdentifierTest, identifierRegistryProvisionalFirst) {
    testUtils::TestLog log;

    babelwires::IdentifierRegistry identifierRegistry;

    babelwires::Identifier hello("Hello");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::Identifier id = identifierRegistry.addIdentifierWithMetadata(
        hello, "Hello World", uuid, babelwires::IdentifierRegistry::Authority::isProvisional);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(identifierRegistry.getName(id), "Hello World");

    // A provisional name updates to match an authorative one.
    babelwires::Identifier hello1("Hello");
    EXPECT_EQ(hello, hello1);
    const babelwires::Identifier id1 = identifierRegistry.addIdentifierWithMetadata(
        hello1, "Hello World 2", uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(id1, hello1);
    EXPECT_EQ(id1.getDiscriminator(), id.getDiscriminator());

    // The original ID will now obtain the new _authoritative_ name.
    EXPECT_EQ(identifierRegistry.getName(id), "Hello World 2");
}

TEST(IdentifierTest, identifierRegistrySerializationDeserialization) {
    testUtils::TestLog log;
    std::string serializedContents;

    babelwires::Identifier id0("hello");
    babelwires::Identifier id1("byebye");
    const std::string name0 = "Name 0";
    const std::string name1 = "Name 1";
    {
        babelwires::IdentifierRegistry identifierRegistry;

        const babelwires::Uuid uuid0("00000000-1111-2222-3333-000000000001");
        const babelwires::Uuid uuid1("00000000-1111-2222-3333-000000000002");

        id0 = identifierRegistry.addIdentifierWithMetadata(id0, name0, uuid0,
                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        id1 =
            identifierRegistry.addIdentifierWithMetadata(id1, name1, uuid1, babelwires::IdentifierRegistry::Authority::isProvisional);

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(identifierRegistry);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        babelwires::AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto fieldNameRegPtr = deserializer.deserializeObject<babelwires::IdentifierRegistry>();
        ASSERT_NE(fieldNameRegPtr, nullptr);
        deserializer.finalize();

        EXPECT_EQ(fieldNameRegPtr->getName(id0), name0);
        EXPECT_EQ(fieldNameRegPtr->getName(id1), name1);
    }
}

TEST(IdentifierTest, implicitIdentifierRegistration) {
    testUtils::TestLog log;

    // Work with a temporary global registry.
    babelwires::IdentifierRegistryScope identifierRegistry;

    int discriminator = 0;
    for (int i = 0; i < 3; ++i) {
        // Repeating this line of code should be a NOOP.
        babelwires::Identifier hello = REGISTERED_ID("hello", "Hello world", "00000000-1111-2222-3333-000000000001");
        EXPECT_EQ(hello, "hello");
        EXPECT_NE(hello.getDiscriminator(), 0);
        if (discriminator != 0) {
            EXPECT_EQ(hello.getDiscriminator(), discriminator);
        }
        discriminator = hello.getDiscriminator();

        EXPECT_EQ(babelwires::IdentifierRegistry::read()->getName(hello), "Hello world");
    }
}

TEST(IdentifierTest, implicitIdentifierRegistrationVector) {
    testUtils::TestLog log;

    // Work with a temporary global registry.
    babelwires::IdentifierRegistryScope identifierRegistry;

    const babelwires::IdentifiersSource source = {
        {"hello", "Hello world", "00000000-1111-2222-3333-000000000001"},
        {"byebye", "Goodbye world", "00000000-1111-2222-3333-000000000002"}};

    int hello_discriminator = 0;
    int goodbye_discriminator = 0;
    for (int i = 0; i < 3; ++i) {
        // Repeating this line of code should be a NOOP.
        babelwires::RegisteredIdentifiers ids = REGISTERED_ID_VECTOR(source);
        EXPECT_EQ(ids.size(), 2);

        babelwires::Identifier hello = ids[0];
        EXPECT_EQ(hello, "hello");
        EXPECT_NE(hello.getDiscriminator(), 0);
        if (hello_discriminator != 0) {
            EXPECT_EQ(hello.getDiscriminator(), hello_discriminator);
        }
        hello_discriminator = hello.getDiscriminator();
        EXPECT_EQ(babelwires::IdentifierRegistry::read()->getName(hello), "Hello world");

        babelwires::Identifier goodbye = ids[1];
        EXPECT_EQ(goodbye, "byebye");
        EXPECT_NE(goodbye.getDiscriminator(), 0);
        if (goodbye_discriminator != 0) {
            EXPECT_EQ(goodbye.getDiscriminator(), goodbye_discriminator);
        }
        goodbye_discriminator = goodbye.getDiscriminator();
        EXPECT_EQ(babelwires::IdentifierRegistry::read()->getName(goodbye), "Goodbye world");
    }
}

TEST(IdentifierTest, longIdentifierInRegistry) {
    testUtils::TestLog log;

    babelwires::IdentifierRegistry identifierRegistry;

    babelwires::LongIdentifier hello("Hello_world");
    EXPECT_EQ(hello.getDiscriminator(), 0);

    babelwires::Uuid uuid("00000000-1111-2222-3333-000000000001");

    const babelwires::LongIdentifier id = identifierRegistry.addIdentifierWithMetadata(
        hello, "Hello World", uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id.getDiscriminator(), 0);
    EXPECT_EQ(identifierRegistry.getName(id), "Hello World");

    babelwires::LongIdentifier hello2("Hello_world");
    babelwires::Uuid uuid2("00000000-1111-2222-3333-000000000002");

    const babelwires::LongIdentifier id2 = identifierRegistry.addIdentifierWithMetadata(
        hello2, "Hello World 2", uuid2, babelwires::IdentifierRegistry::Authority::isAuthoritative);
    EXPECT_EQ(hello, id);
    EXPECT_NE(id2.getDiscriminator(), 0);
    EXPECT_NE(id2.getDiscriminator(), id.getDiscriminator());
    EXPECT_EQ(identifierRegistry.getName(id2), "Hello World 2");
}
