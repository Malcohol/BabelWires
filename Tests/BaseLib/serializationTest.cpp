#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Tests/TestUtils/testLog.hpp>

#include <gtest/gtest.h>

#include <array>

using namespace babelwires;
using namespace testUtils;

namespace {
    struct A : Serializable {
        SERIALIZABLE(A, "A", void, 1);

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeValue("x", m_x);
            serializer.serializeValueArray("array", m_array);
        }

        void deserializeContents(Deserializer& deserializer) override {
            THROW_ON_ERROR(deserializer.deserializeValue("x", m_x), ParseException);
            if (auto itResult = deserializer.deserializeValueArray<std::string>("array")) {
                for (auto& it = *itResult; it.isValid(); ++it) {
                    m_array.emplace_back(std::move(it.deserializeValue()));
                }
            }
        }

        int m_x = 0;
        std::vector<std::string> m_array;
    };
} // namespace

TEST(SerializationTest, values) {
    std::string serializedContents;
    {
        A a;
        a.m_x = 12;
        a.m_array.emplace_back("Hello");
        a.m_array.emplace_back("Goodbye");

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(a);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto APtr = deserializer.deserializeObject<A>();
        deserializer.finalize();

        EXPECT_EQ(APtr->m_x, 12);
        EXPECT_EQ(APtr->m_array.size(), 2);
        EXPECT_EQ(APtr->m_array[0], "Hello");
        EXPECT_EQ(APtr->m_array[1], "Goodbye");
    }
}

namespace {
    struct B : Serializable {
        SERIALIZABLE(B, "B", void, 1);

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeObject(m_a);
            serializer.serializeArray("arrayOfAs", m_arrayOfAs);
        }

        void deserializeContents(Deserializer& deserializer) override {
            if (std::unique_ptr<A> a = deserializer.deserializeObject<A>()) {
                m_a = std::move(*a);
            }
            auto itResult = deserializer.deserializeArray<A>("arrayOfAs");
            THROW_ON_ERROR(itResult, ParseException);
            for (auto& it = *itResult; it.isValid(); ++it) {
                auto result = it.getObject();
                THROW_ON_ERROR(result, ParseException);
                m_arrayOfAs.emplace_back(std::move(**result));
            }
        }

        A m_a;
        std::vector<A> m_arrayOfAs;
    };
} // namespace

TEST(SerializationTest, objects) {
    std::string serializedContents;
    {
        B b;
        b.m_a.m_x = 12;
        b.m_a.m_array.emplace_back("Hello");
        b.m_arrayOfAs.emplace_back();
        b.m_arrayOfAs[0].m_x = 22;
        b.m_arrayOfAs[0].m_array.emplace_back("Goodbye");
        b.m_arrayOfAs.emplace_back();
        b.m_arrayOfAs[1].m_x = 100;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(b);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto BPtr = deserializer.deserializeObject<B>();
        deserializer.finalize();

        EXPECT_EQ(BPtr->m_a.m_x, 12);
        EXPECT_EQ(BPtr->m_a.m_array.size(), 1);
        EXPECT_EQ(BPtr->m_a.m_array[0], "Hello");
        EXPECT_EQ(BPtr->m_arrayOfAs.size(), 2);
        EXPECT_EQ(BPtr->m_arrayOfAs[0].m_x, 22);
        EXPECT_EQ(BPtr->m_arrayOfAs[0].m_array.size(), 1);
        EXPECT_EQ(BPtr->m_arrayOfAs[0].m_array[0], "Goodbye");
        EXPECT_EQ(BPtr->m_arrayOfAs[1].m_x, 100);
    }
}

namespace {
#define WILL_BE_PATCHED_TO_C "WILL_BE_PATCHED_TO_C"

    namespace old {
        /// The original version of the class C.
        struct C : Serializable {
            SERIALIZABLE(C, WILL_BE_PATCHED_TO_C, void, 1);
            // We will rewrite this in the file to "C"

            void serializeContents(Serializer& serializer) const override { serializer.serializeValue("x", m_x); }

            void deserializeContents(Deserializer& deserializer) override { 
                THROW_ON_ERROR(deserializer.deserializeValue("x", m_x), ParseException); 
            }

            // A refactor will split this into a sign and an unsigened int.
            int m_x = 0;
        };
    } // namespace old

    namespace current {
        /// The version of C after a refactor.
        /// We want to be able to deserialize older and current versions.
        struct C : Serializable {
            SERIALIZABLE(C, "C", void, 2);

            void serializeContents(Serializer& serializer) const override {
                serializer.serializeValue("isPositive", m_isPositive);
                serializer.serializeValue("x", m_x);
            }

            void deserializeContents(Deserializer& deserializer) override {
                const int version = deserializer.getTypeVersion("C");
                if (version == 1) {
                    int oldInt = 0;
                    THROW_ON_ERROR(deserializer.deserializeValue("x", oldInt), ParseException);
                    m_isPositive = (oldInt >= 0);
                    m_x = std::abs(oldInt);
                } else {
                    // Current versions.
                    // This case is also used for later versions and version 0, but the serialization system will have
                    // warned.
                    THROW_ON_ERROR(deserializer.deserializeValue("isPositive", m_isPositive), ParseException);
                    THROW_ON_ERROR(deserializer.deserializeValue("x", m_x), ParseException);
                }
            }

            bool m_isPositive = true;
            unsigned int m_x = 0;
        };
    } // namespace current
} // namespace

// Test how the system supports version from an old class.
TEST(SerializationTest, versioningOld) {
    std::string serializedContents;
    {
        old::C c;
        c.m_x = -2;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(c);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        size_t pos = 0;
        while (pos = serializedContents.find(WILL_BE_PATCHED_TO_C, pos), pos != std::string::npos) {
            serializedContents.replace(pos, std::strlen(WILL_BE_PATCHED_TO_C), "C");
        }
    }

    {
        TestLog log;
        AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto CPtr = deserializer.deserializeObject<current::C>();
        deserializer.finalize();

        EXPECT_EQ(CPtr->m_isPositive, false);
        EXPECT_EQ(CPtr->m_x, 2);
    }
}

// Confirm that version support doesn't affect normal serialization
TEST(SerializationTest, versioningCurrent) {
    std::string serializedContents;
    {
        current::C c;
        c.m_isPositive = false;
        c.m_x = 18;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(c);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto CPtr = deserializer.deserializeObject<current::C>();
        deserializer.finalize();

        EXPECT_EQ(CPtr->m_isPositive, false);
        EXPECT_EQ(CPtr->m_x, 18);
    }
}

namespace {
    struct Base : babelwires::Serializable {
        SERIALIZABLE_ABSTRACT(Base, void);
    };

    struct Concrete0 : Base {
        SERIALIZABLE(Concrete0, "Concrete0", Base, 1)

        void serializeContents(Serializer& serializer) const override { serializer.serializeValue("x", m_x); }

        void deserializeContents(Deserializer& deserializer) override { 
            THROW_ON_ERROR(deserializer.deserializeValue("x", m_x), ParseException); 
        }

        int m_x = 0;
    };

    struct Intermediate : Base {
        SERIALIZABLE_ABSTRACT(Intermediate, Base);
    };

    struct Intermediate2 : Intermediate {};

    struct Concrete1 : Intermediate2 {
        SERIALIZABLE(Concrete1, "Concrete1", Intermediate2, 1);

        void serializeContents(Serializer& serializer) const override { serializer.serializeValue("s", m_s); }

        void deserializeContents(Deserializer& deserializer) override { 
            THROW_ON_ERROR(deserializer.deserializeValue("s", m_s), ParseException); 
        }

        std::string m_s;
    };

    struct Concrete2 : Concrete1 {
        SERIALIZABLE(Concrete2, "Concrete2", Concrete1, 1);
        void serializeContents(Serializer& serializer) const override {
            Concrete1::serializeContents(serializer);
            serializer.serializeValue("u32", m_u32);
        }

        void deserializeContents(Deserializer& deserializer) override {
            Concrete1::deserializeContents(deserializer);
            THROW_ON_ERROR(deserializer.deserializeValue("u32", m_u32), ParseException);
        }

        std::uint32_t m_u32;
    };

    struct Main : babelwires::Serializable {
        SERIALIZABLE(Main, "Main", void, 1);

        void serializeContents(Serializer& serializer) const override {
            if (m_base) {
                serializer.serializeObject(*m_base, "base");
            }
            if (m_concrete0) {
                serializer.serializeObject(*m_concrete0, "concrete0");
            }
            if (m_intermediate) {
                serializer.serializeObject(*m_intermediate, "intermediate");
            }
            if (m_concrete1) {
                serializer.serializeObject(*m_concrete1, "concrete1");
            }
            if (m_concrete2) {
                serializer.serializeObject(*m_concrete2, "concrete2");
            }
            serializer.serializeArray("objects", m_objects);
        }

        void deserializeContents(Deserializer& deserializer) override {
            m_base = deserializer.deserializeObject<Base>("base", babelwires::Deserializer::IsOptional::Optional);
            m_concrete0 =
                deserializer.deserializeObject<Concrete0>("concrete0", babelwires::Deserializer::IsOptional::Optional);
            m_intermediate = deserializer.deserializeObject<Intermediate>(
                "intermediate", babelwires::Deserializer::IsOptional::Optional);
            m_concrete1 =
                deserializer.deserializeObject<Concrete1>("concrete1", babelwires::Deserializer::IsOptional::Optional);
            m_concrete2 =
                deserializer.deserializeObject<Concrete2>("concrete2", babelwires::Deserializer::IsOptional::Optional);
            if (auto itResult = deserializer.deserializeArray<Base>("objects")) {
                for (auto& it = *itResult; it.isValid(); ++it) {
                    auto result = it.getObject();
                    THROW_ON_ERROR(result, ParseException);
                    m_objects.emplace_back(std::move(*result));
                }
            }
        }

        std::unique_ptr<Base> m_base;
        std::unique_ptr<Concrete0> m_concrete0;
        std::unique_ptr<Intermediate> m_intermediate;
        std::unique_ptr<Concrete1> m_concrete1;
        std::unique_ptr<Concrete2> m_concrete2;
        std::vector<std::unique_ptr<Base>> m_objects;
    };
} // namespace

TEST(SerializationTest, polymorphism) {
    std::string serializedContents;
    {
        Main m;
        m.m_base = std::make_unique<Concrete0>();
        static_cast<Concrete0*>(m.m_base.get())->m_x = 32;
        m.m_concrete0 = std::make_unique<Concrete0>();
        m.m_concrete0->m_x = -13;
        m.m_intermediate = std::make_unique<Concrete1>();
        static_cast<Concrete1*>(m.m_intermediate.get())->m_s = "Jump!";
        m.m_concrete1 = std::make_unique<Concrete1>();
        m.m_concrete1->m_s = "Ergh";
        m.m_concrete2 = std::make_unique<Concrete2>();
        m.m_concrete2->m_u32 = 0x12345678;
        m.m_objects.emplace_back(std::make_unique<Concrete1>());
        static_cast<Concrete1*>(m.m_objects[0].get())->m_s = "Tuesday";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(m);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        AutomaticDeserializationRegistry deserializationReg;
        babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
        auto MainPtr = deserializer.deserializeObject<Main>();
        deserializer.finalize();

        ASSERT_NE(MainPtr->m_base, nullptr);
        EXPECT_NE(dynamic_cast<Concrete0*>(MainPtr->m_base.get()), nullptr);
        EXPECT_EQ(dynamic_cast<Concrete0*>(MainPtr->m_base.get())->m_x, 32);
        ASSERT_NE(MainPtr->m_concrete0, nullptr);
        EXPECT_EQ(MainPtr->m_concrete0->m_x, -13);
        EXPECT_NE(MainPtr->m_intermediate, nullptr);
        ASSERT_NE(dynamic_cast<Concrete1*>(MainPtr->m_intermediate.get()), nullptr);
        EXPECT_EQ(dynamic_cast<Concrete1*>(MainPtr->m_intermediate.get())->m_s, "Jump!");
        ASSERT_NE(MainPtr->m_concrete1, nullptr);
        EXPECT_EQ(MainPtr->m_concrete1->m_s, "Ergh");
        ASSERT_NE(MainPtr->m_concrete2, nullptr);
        EXPECT_EQ(MainPtr->m_concrete2->m_u32, 0x12345678);

        ASSERT_EQ(MainPtr->m_objects.size(), 1);
        ASSERT_NE(dynamic_cast<Concrete1*>(MainPtr->m_objects[0].get()), nullptr);
        EXPECT_EQ(dynamic_cast<Concrete1*>(MainPtr->m_objects[0].get())->m_s, "Tuesday");
    }
}

TEST(SerializationTest, polymorphismFail) {
    std::string serializedContents;
    {
        Main m;
        m.m_concrete2 = std::make_unique<Concrete2>();
        m.m_concrete2->m_s = "Ergh";
        m.m_concrete2->m_u32 = 145;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(m);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    // Replace Concrete2 by an unrelated type, a distanct relation and its parent type.
    // All of these scenarios should throw, since they are not subtypes of Concrete2.
    for (const auto& t : std::array<std::string_view, 3>{"A", "Concrete0", "Concrete1"}) {
        std::string serializedContents2 = serializedContents;

        {
            size_t pos = 0;
            while (pos = serializedContents2.find("Concrete2", pos), pos != std::string::npos) {
                serializedContents2.replace(pos, std::strlen("Concrete2"), t);
            }
        }

        {
            TestLog log;
            AutomaticDeserializationRegistry deserializationReg;
            try {
                babelwires::XmlDeserializer deserializer(serializedContents2, deserializationReg, log);
                auto MainPtr = deserializer.deserializeObject<Main>();
                deserializer.finalize();
            } catch (const ParseException& e) {
                EXPECT_NE(std::string_view(e.what()).find(t), std::string_view::npos);
            } catch (...) {
                EXPECT_TRUE(false);
            }
        }
    }
}
