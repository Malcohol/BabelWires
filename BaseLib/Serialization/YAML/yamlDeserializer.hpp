/**
 * The YamlDeserializer implements the Deserializer and loads data from a YAML representation.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/Serialization/deserializer.hpp>

#include <set>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace babelwires {

    class BASELIB_API YamlDeserializer : public Deserializer {
      public:
        YamlDeserializer(const DeserializationRegistryInterface& deserializationRegistry, UserLogger& userLogger);

        Result parse(std::string_view yamlText) override;

        ResultT<bool> tryDeserializeValue(std::string_view key, bool& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::string& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint64_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint32_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint16_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint8_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int64_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int32_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int16_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int8_t& value) override;

      protected:
        struct BASELIB_API IteratorImpl : Deserializer::AbstractIterator {
            IteratorImpl(YamlDeserializer& deserializer, const YAML::Node& arrayNode);

            Result advance() override;
            bool isValid() const override;

            YamlDeserializer& m_deserializer;
            YAML::Node m_arrayNode;
            std::size_t m_index = 0;
        };

        std::unique_ptr<AbstractIterator> getIteratorImpl() override;

        bool pushObject(std::string_view key) override;
        Result popObject() override;

        bool pushArray(std::string_view key) override;
        Result popArray() override;

        std::string_view getCurrentTypeName() override;
        bool currentValueArrayElementMatchesType(std::string_view expectedTypeName) override;
        ErrorStorage addContextDescription(const ErrorStorage& e) const override;

      private:
        friend IteratorImpl;

        void contextPush(std::string_view key, const YAML::Node& node, bool isArray, bool isArrayElement);
        Result contextPop();
        const YAML::Node& getCurrentNode() const;
        void keyWasQueried(std::string_view key);

        ResultT<bool> tryGetValueNode(std::string_view key, YAML::Node& valueNode);

        template <typename INT_TYPE>
        ResultT<bool> getIntValue(std::string_view key, INT_TYPE& value);

        struct ContextEntry {
            YAML::Node m_node;
            bool m_isArray = false;
            bool m_isArrayElement = false;
            std::string m_key;
            std::set<std::string, std::less<>> m_keysQueried;
        };

        YAML::Node m_documentNode;
        std::vector<ContextEntry> m_yamlContext;
    };

} // namespace babelwires