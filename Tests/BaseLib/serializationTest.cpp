#include <BaseLib/Serialization/deserializationRegistry.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <BaseLib/Utilities/downcastable.hpp>

#include <Tests/TestUtils/testLog.hpp>

#include <gtest/gtest.h>

#include <array>

using namespace babelwires;
using namespace testUtils;

namespace {
    struct XmlSerializationBackend {
        static std::unique_ptr<Serializer> createSerializer() {
            return std::make_unique<XmlSerializer>();
        }

        static std::unique_ptr<Deserializer> createDeserializer(const DeserializationRegistry& deserializationRegistry,
                                                                TestLog& log) {
            return std::make_unique<XmlDeserializer>(deserializationRegistry, log);
        }

        // Rewrite runtime type tokens in serialized XML so backend-agnostic tests can exercise type-mismatch cases.
        static std::string rewriteTypeInData(std::string serializedContents, std::string_view fromType,
                                             std::string_view toType) {
            size_t pos = 0;
            while ((pos = serializedContents.find(fromType, pos)) != std::string::npos) {
                serializedContents.replace(pos, fromType.size(), toType);
                pos += toType.size();
            }
            return serializedContents;
        }
    };
}

namespace {
    struct A : Serializable {
        SERIALIZABLE(A, "A", void, 1);

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeValue("x", m_x);
            serializer.serializeValueArray("array", m_array);
        }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeValue("x", m_x));
            if (auto it = deserializer.tryDeserializeValueArray<std::string>("array")) {
                while (it->isValid()) {
                    ASSIGN_OR_ERROR(m_array.emplace_back(), it->deserializeValue());
                    DO_OR_ERROR(it->advance());
                }
            }
            return {};
        }

        int m_x = 0;
        std::vector<std::string> m_array;
    };

    struct AWithUnexpectedValue : A {
        // Intentionally keep the same wire type as A so this helper can produce extra data that still deserializes as A.
        SERIALIZABLE(AWithUnexpectedValue, "A", A, 1);

        void serializeContents(Serializer& serializer) const override {
            A::serializeContents(serializer);
            serializer.serializeValue("unexpected", m_unexpected);
        }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(A::deserializeContents(deserializer));
            DO_OR_ERROR(deserializer.deserializeValue("unexpected", m_unexpected));
            return {};
        }

        int m_unexpected = 0;
    };
} // namespace

template <typename TBackend> class SerializationBackendTest : public ::testing::Test {};

using SerializationBackendTypes = ::testing::Types<XmlSerializationBackend>;
TYPED_TEST_SUITE(SerializationBackendTest, SerializationBackendTypes);

TYPED_TEST(SerializationBackendTest, values) {
    std::string serializedContents;
    {
        A a;
        a.m_x = 12;
        a.m_array.emplace_back("Hello");
        a.m_array.emplace_back("Goodbye");

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(a);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<A>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto APtrResult = deserializer->template deserializeObject<A>();
        ASSERT_TRUE(APtrResult);
        auto APtr = std::move(*APtrResult);
        ASSERT_TRUE(deserializer->finalize());

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

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeObjectByValue<A>(m_a));
            ASSIGN_OR_ERROR(auto it, deserializer.deserializeArray<A>("arrayOfAs"));
            while (it.isValid()) {
                ASSIGN_OR_ERROR(auto result, it.getObject());
                m_arrayOfAs.emplace_back(std::move(*result));
                DO_OR_ERROR(it.advance());
            }
            return {};
        }

        A m_a;
        std::vector<A> m_arrayOfAs;
    };

    struct KeyedContainer : Serializable {
        SERIALIZABLE(KeyedContainer, "KeyedContainer", void, 1);

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeObject(m_defaultKeyObject);
            serializer.serializeObject(m_explicitKeyObject, "renamedA");
        }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeObjectByValue(m_defaultKeyObject));
            DO_OR_ERROR(deserializer.deserializeObjectByValue(m_explicitKeyObject, "renamedA"));
            return {};
        }

        A m_defaultKeyObject;
        A m_explicitKeyObject;
    };

    struct CustomValueArrayContainer : Serializable {
        SERIALIZABLE(CustomValueArrayContainer, "CustomValueArrayContainer", void, 1);

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeValueArray("values", m_values, "item");
        }

        Result deserializeContents(Deserializer& deserializer) override {
            ASSIGN_OR_ERROR(auto it, deserializer.deserializeValueArray<std::string>("values", "item"));
            while (it.isValid()) {
                ASSIGN_OR_ERROR(m_values.emplace_back(), it.deserializeValue());
                DO_OR_ERROR(it.advance());
            }
            return {};
        }

        std::vector<std::string> m_values;
    };
} // namespace

TYPED_TEST(SerializationBackendTest, explicitAndDefaultKeys) {
    std::string serializedContents;
    {
        KeyedContainer container;
        container.m_defaultKeyObject.m_x = 17;
        container.m_defaultKeyObject.m_array = {"default"};
        container.m_explicitKeyObject.m_x = 23;
        container.m_explicitKeyObject.m_array = {"explicit", "key"};

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(container);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<A>();
        deserializationReg.registerClass<KeyedContainer>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto containerResult = deserializer->template deserializeObject<KeyedContainer>();
        ASSERT_TRUE(containerResult);
        auto container = std::move(*containerResult);
        ASSERT_TRUE(deserializer->finalize());

        EXPECT_EQ(container->m_defaultKeyObject.m_x, 17);
        ASSERT_EQ(container->m_defaultKeyObject.m_array.size(), 1);
        EXPECT_EQ(container->m_defaultKeyObject.m_array[0], "default");
        EXPECT_EQ(container->m_explicitKeyObject.m_x, 23);
        ASSERT_EQ(container->m_explicitKeyObject.m_array.size(), 2);
        EXPECT_EQ(container->m_explicitKeyObject.m_array[0], "explicit");
        EXPECT_EQ(container->m_explicitKeyObject.m_array[1], "key");
    }
}

TYPED_TEST(SerializationBackendTest, valueArraysAllowCustomElementNames) {
    std::string serializedContents;
    {
        CustomValueArrayContainer container;
        container.m_values = {"one", "two", "three"};

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(container);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<CustomValueArrayContainer>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto containerResult = deserializer->template deserializeObject<CustomValueArrayContainer>();
        ASSERT_TRUE(containerResult);
        auto container = std::move(*containerResult);
        ASSERT_TRUE(deserializer->finalize());

        ASSERT_EQ(container->m_values.size(), 3);
        EXPECT_EQ(container->m_values[0], "one");
        EXPECT_EQ(container->m_values[1], "two");
        EXPECT_EQ(container->m_values[2], "three");
    }
}

TYPED_TEST(SerializationBackendTest, objects) {
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

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(b);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<A>();
        deserializationReg.registerClass<B>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto BPtrResult = deserializer->template deserializeObject<B>();
        ASSERT_TRUE(BPtrResult);
        auto BPtr = std::move(*BPtrResult);
        ASSERT_TRUE(deserializer->finalize());

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
    namespace old {
        /// The original version of the class C.
        struct C : Serializable {
            SERIALIZABLE(C, "C", void, 1);

            void serializeContents(Serializer& serializer) const override { serializer.serializeValue("x", m_x); }

            Result deserializeContents(Deserializer& deserializer) override {
                DO_OR_ERROR(deserializer.deserializeValue("x", m_x));
                return {};
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

            Result deserializeContents(Deserializer& deserializer) override {
                const int version = deserializer.getTypeVersion("C");
                if (version == 1) {
                    int oldInt = 0;
                    DO_OR_ERROR(deserializer.deserializeValue("x", oldInt));
                    m_isPositive = (oldInt >= 0);
                    m_x = std::abs(oldInt);
                } else {
                    // Current versions.
                    // This case is also used for later versions and version 0, but the serialization system will have
                    // warned.
                    DO_OR_ERROR(deserializer.deserializeValue("isPositive", m_isPositive));
                    DO_OR_ERROR(deserializer.deserializeValue("x", m_x));
                }
                return {};
            }

            bool m_isPositive = true;
            unsigned int m_x = 0;
        };
    } // namespace current
} // namespace

// Test how the system supports version from an old class.
TYPED_TEST(SerializationBackendTest, versioningOld) {
    std::string serializedContents;
    {
        old::C c;
        c.m_x = -2;

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(c);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<current::C>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto CPtrResult = deserializer->template deserializeObject<current::C>();
        ASSERT_TRUE(CPtrResult);
        auto CPtr = std::move(*CPtrResult);
        ASSERT_TRUE(deserializer->finalize());

        EXPECT_EQ(CPtr->m_isPositive, false);
        EXPECT_EQ(CPtr->m_x, 2);
    }
}

// Confirm that version support doesn't affect normal serialization
TYPED_TEST(SerializationBackendTest, versioningCurrent) {
    std::string serializedContents;
    {
        current::C c;
        c.m_isPositive = false;
        c.m_x = 18;

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(c);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<current::C>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto CPtrResult = deserializer->template deserializeObject<current::C>();
        ASSERT_TRUE(CPtrResult);
        auto CPtr = std::move(*CPtrResult);
        ASSERT_TRUE(deserializer->finalize());

        EXPECT_EQ(CPtr->m_isPositive, false);
        EXPECT_EQ(CPtr->m_x, 18);
    }
}

namespace {
    struct Base : babelwires::Serializable {
        DOWNCASTABLE_BASE(Base);
        SERIALIZABLE_ABSTRACT(Base, void);
    };

    struct Concrete0 : Base {
        DOWNCASTABLE(Concrete0, Base);
        SERIALIZABLE(Concrete0, "Concrete0", Base, 1)

        void serializeContents(Serializer& serializer) const override { serializer.serializeValue("x", m_x); }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeValue("x", m_x));
            return {};
        }

        int m_x = 0;
    };

    struct Intermediate : Base {
        DOWNCASTABLE(Intermediate, Base);
        SERIALIZABLE_ABSTRACT(Intermediate, Base);
    };

    struct Intermediate2 : Intermediate {};

    struct Concrete1 : Intermediate2 {
        DOWNCASTABLE(Concrete1, Intermediate);
        SERIALIZABLE(Concrete1, "Concrete1", Intermediate2, 1);

        void serializeContents(Serializer& serializer) const override { serializer.serializeValue("s", m_s); }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeValue("s", m_s));
            return {};
        }

        std::string m_s;
    };

    struct Concrete2 : Concrete1 {
        DOWNCASTABLE(Concrete2, Concrete1);
        SERIALIZABLE(Concrete2, "Concrete2", Concrete1, 1);
        void serializeContents(Serializer& serializer) const override {
            Concrete1::serializeContents(serializer);
            serializer.serializeValue("u32", m_u32);
        }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(Concrete1::deserializeContents(deserializer));
            DO_OR_ERROR(deserializer.deserializeValue("u32", m_u32));
            return {};
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

        Result deserializeContents(Deserializer& deserializer) override {
            ASSIGN_OR_ERROR(m_base, deserializer.tryDeserializeObject<Base>("base"));
            ASSIGN_OR_ERROR(m_concrete0, deserializer.tryDeserializeObject<Concrete0>("concrete0"));
            ASSIGN_OR_ERROR(m_intermediate, deserializer.tryDeserializeObject<Intermediate>("intermediate"));
            ASSIGN_OR_ERROR(m_concrete1, deserializer.tryDeserializeObject<Concrete1>("concrete1"));
            ASSIGN_OR_ERROR(m_concrete2, deserializer.tryDeserializeObject<Concrete2>("concrete2"));
            ASSIGN_OR_ERROR(auto it, deserializer.deserializeArray<Base>("objects"));
            while (it.isValid()) {
                ASSIGN_OR_ERROR(auto ptr, it.getObject());
                m_objects.emplace_back(std::move(ptr));
                DO_OR_ERROR(it.advance());
            }
            return {};
        }

        std::unique_ptr<Base> m_base;
        std::unique_ptr<Concrete0> m_concrete0;
        std::unique_ptr<Intermediate> m_intermediate;
        std::unique_ptr<Concrete1> m_concrete1;
        std::unique_ptr<Concrete2> m_concrete2;
        std::vector<std::unique_ptr<Base>> m_objects;
    };

    struct ByValueSubtypeWriter : babelwires::Serializable {
        SERIALIZABLE(ByValueSubtypeWriter, "ByValueHolder", void, 1);

        void serializeContents(Serializer& serializer) const override { serializer.serializeObject(m_value, "value"); }

        Result deserializeContents(Deserializer& deserializer) override {
            ASSIGN_OR_ERROR(m_unusedValue, deserializer.tryDeserializeObject<Concrete2>("value"));
            return {};
        }

        Concrete2 m_value;
        std::unique_ptr<Concrete2> m_unusedValue;
    };

    struct ByValueSubtypeReader : babelwires::Serializable {
        SERIALIZABLE(ByValueSubtypeReader, "ByValueHolder", void, 1);

        void serializeContents(Serializer& serializer) const override { serializer.serializeObject(m_value, "value"); }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeObjectByValue(m_value, "value"));
            return {};
        }

        Concrete1 m_value;
    };
} // namespace

TYPED_TEST(SerializationBackendTest, polymorphism) {
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

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(m);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<A>();
        deserializationReg.registerClass<Main>();
        deserializationReg.registerClass<Concrete0>();
        deserializationReg.registerClass<Concrete1>();
        deserializationReg.registerClass<Concrete2>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto MainPtrResult = deserializer->template deserializeObject<Main>();
        ASSERT_TRUE(MainPtrResult);
        auto MainPtr = std::move(*MainPtrResult);
        ASSERT_TRUE(deserializer->finalize());

        ASSERT_NE(MainPtr->m_base, nullptr);
        EXPECT_NE(MainPtr->m_base->template tryAs<Concrete0>(), nullptr);
        EXPECT_EQ(MainPtr->m_base->template as<Concrete0>().m_x, 32);
        ASSERT_NE(MainPtr->m_concrete0, nullptr);
        EXPECT_EQ(MainPtr->m_concrete0->m_x, -13);
        EXPECT_NE(MainPtr->m_intermediate, nullptr);
        ASSERT_NE(MainPtr->m_intermediate->template tryAs<Concrete1>(), nullptr);
        EXPECT_EQ(MainPtr->m_intermediate->template as<Concrete1>().m_s, "Jump!");
        ASSERT_NE(MainPtr->m_concrete1, nullptr);
        EXPECT_EQ(MainPtr->m_concrete1->m_s, "Ergh");
        ASSERT_NE(MainPtr->m_concrete2, nullptr);
        EXPECT_EQ(MainPtr->m_concrete2->m_u32, 0x12345678);

        ASSERT_EQ(MainPtr->m_objects.size(), 1);
        ASSERT_NE(MainPtr->m_objects[0]->template tryAs<Concrete1>(), nullptr);
        EXPECT_EQ(MainPtr->m_objects[0]->template as<Concrete1>().m_s, "Tuesday");
    }
}

TYPED_TEST(SerializationBackendTest, polymorphismFail) {
    std::string serializedContents;
    {
        Main m;
        m.m_concrete2 = std::make_unique<Concrete2>();
        m.m_concrete2->m_s = "Ergh";
        m.m_concrete2->m_u32 = 145;

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(m);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    // Replace Concrete2 by an unrelated type, a distant relation and its parent type.
    // All of these scenarios should fail, since they are not subtypes of Concrete2.
    for (const auto& t : std::array<std::string_view, 3>{"A", "Concrete0", "Concrete1"}) {
        std::string serializedContents2 = TypeParam::rewriteTypeInData(serializedContents, "Concrete2", t);

        {
            TestLog log;
            DeserializationRegistry deserializationReg;
            deserializationReg.registerClass<A>();
            deserializationReg.registerClass<Main>();
            deserializationReg.registerClass<Concrete0>();
            deserializationReg.registerClass<Concrete1>();
            deserializationReg.registerClass<Concrete2>();
            auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
            ASSERT_NE(deserializer, nullptr);
            ASSERT_TRUE(deserializer->parse(serializedContents2));
            auto MainPtrResult = deserializer->template deserializeObject<Main>();
            EXPECT_FALSE(MainPtrResult);
            EXPECT_NE(std::string_view(MainPtrResult.error().toString()).find(t), std::string_view::npos);
            deserializer->finalize();
        }
    }
}

TYPED_TEST(SerializationBackendTest, deserializeObjectByValueRejectsStrictSubclasses) {
    std::string serializedContents;
    {
        ByValueSubtypeWriter writer;
        writer.m_value.m_s = "strict subtype";
        writer.m_value.m_u32 = 77;

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(writer);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<ByValueSubtypeReader>();
        deserializationReg.registerClass<Concrete1>();
        deserializationReg.registerClass<Concrete2>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto readerResult = deserializer->template deserializeObject<ByValueSubtypeReader>();
        ASSERT_FALSE(readerResult);
        EXPECT_NE(std::string_view(readerResult.error().toString()).find("Concrete2"), std::string_view::npos);
        EXPECT_NE(std::string_view(readerResult.error().toString()).find("Concrete1"), std::string_view::npos);
        deserializer->finalizeOnError();
    }
}

TYPED_TEST(SerializationBackendTest, deserializeRejectsUnexpectedUnconsumedData) {
    std::string serializedContents;
    {
        AWithUnexpectedValue a;
        a.m_x = 12;
        a.m_unexpected = 13;

        auto serializer = TypeParam::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(a);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    {
        TestLog log;
        DeserializationRegistry deserializationReg;
        deserializationReg.registerClass<A>();
        auto deserializer = TypeParam::createDeserializer(deserializationReg, log);
        ASSERT_NE(deserializer, nullptr);
        ASSERT_TRUE(deserializer->parse(serializedContents));
        auto APtrResult = deserializer->template deserializeObject<A>();
        ASSERT_FALSE(APtrResult);
        EXPECT_NE(std::string_view(APtrResult.error().toString()).find("unexpected"), std::string_view::npos);
        deserializer->finalizeOnError();
    }
}

TEST(XmlSerializationTest, xmlSerializerRejectsReservedMetadataPrefixForObjectKeys) {
    EXPECT_DEATH(
        {
            A a;
            babelwires::XmlSerializer serializer;
            serializer.serializeObject(a, "meta:illegal");
        },
        "reserved for backend metadata");
}

TEST(XmlSerializationTest, xmlSerializerRejectsReservedMetadataPrefixForValueKeys) {
    EXPECT_DEATH(
        {
            babelwires::XmlSerializer serializer;
            serializer.serializeValue("meta:illegal", 17);
        },
        "reserved for backend metadata");
}