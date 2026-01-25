#include <BaseLib/uuid.hpp>

#include <BaseLib/exceptions.hpp>

#include <gtest/gtest.h>

#include <array>

using namespace babelwires;

TEST(UuidTest, basicOperations) {
    Uuid zero;
    EXPECT_TRUE(zero.isZero());

    const char sourceUuidText[] = "2b67299c-0f27-46d4-a8bf-8a97e1557f8f";
    Uuid uuid(sourceUuidText);
    EXPECT_FALSE(uuid.isZero());
    EXPECT_EQ(uuid, Uuid(sourceUuidText));
    EXPECT_NE(uuid, Uuid("f0049bdd-0444-446f-8889-e2194e89cfd7"));
    {
        std::ostringstream os;
        os << uuid;
        EXPECT_EQ(os.str(), sourceUuidText);
    }
    {
        std::string serialized = uuid.serializeToString();
        Uuid uuidD = *Uuid::deserializeFromString(serialized);
        EXPECT_EQ(uuid, uuidD);
    }
}

TEST(UuidTest, badParsing) {
    for (const auto& s : std::array<std::string_view, 5>{
             "", "916d86a6-1d48-4bce-a83f-8deb375a955", "916d86a61d48-4bce-a83f-8deb375a9552",
             "916d86a6-1d48-4bce-a83f-8deb375x9552", "916d86a6-1d48-4bce-a83f-8deb375a955298"}) {
        EXPECT_FALSE(Uuid::deserializeFromString(s).has_value());
    }
}

TEST(UuidTest, randomize) {
    std::default_random_engine randomEngine(1445);

    Uuid uuid;
    uuid.randomize(randomEngine);
    EXPECT_FALSE(uuid.isZero());

    Uuid uuid2;
    uuid2.randomize(randomEngine);
    EXPECT_FALSE(uuid2.isZero());
    // Statistical!
    EXPECT_NE(uuid, uuid2);

    Uuid uuid3;
    randomEngine.seed(1445);
    uuid3.randomize(randomEngine);
    EXPECT_EQ(uuid, uuid3);
}
