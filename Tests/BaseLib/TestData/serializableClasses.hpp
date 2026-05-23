#pragma once

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

#include <vector>

namespace testData {

    struct A : babelwires::Serializable {
        SERIALIZABLE(A, "A", void, 1);

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeValue("x", m_x);
            serializer.serializeValueArray("array", m_array);
        }

        babelwires::Result deserializeContents(babelwires::Deserializer& deserializer) override {
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

    struct KeyedContainer : babelwires::Serializable {
        SERIALIZABLE(KeyedContainer, "KeyedContainer", void, 1);

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeObject(m_defaultKeyObject);
            serializer.serializeObject(m_explicitKeyObject, "renamedA");
        }

        babelwires::Result deserializeContents(babelwires::Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeObjectByValue(m_defaultKeyObject));
            DO_OR_ERROR(deserializer.deserializeObjectByValue(m_explicitKeyObject, "renamedA"));
            return {};
        }

        A m_defaultKeyObject;
        A m_explicitKeyObject;
    };

    struct CustomValueArrayContainer : babelwires::Serializable {
        SERIALIZABLE(CustomValueArrayContainer, "CustomValueArrayContainer", void, 1);

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeValueArray("values", m_values, "item");
        }

        babelwires::Result deserializeContents(babelwires::Deserializer& deserializer) override {
            ASSIGN_OR_ERROR(auto it, deserializer.deserializeValueArray<std::string>("values", "item"));
            while (it.isValid()) {
                ASSIGN_OR_ERROR(m_values.emplace_back(), it.deserializeValue());
                DO_OR_ERROR(it.advance());
            }
            return {};
        }

        std::vector<std::string> m_values;
    };

} // namespace testData