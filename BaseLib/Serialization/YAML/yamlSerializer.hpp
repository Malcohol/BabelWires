/**
 * The YamlSerializer implements Serializer and writes data in a YAML representation.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/Serialization/serializer.hpp>

#include <yaml-cpp/yaml.h>

#include <ostream>
#include <string>
#include <vector>

namespace babelwires {

    class BASELIB_API YamlSerializer : public Serializer {
      public:
        YamlSerializer();

        void write(std::ostream& os) override;

      protected:
        void doPushObject(std::string_view typeName) override;
        void doPushValueArrayElement(std::string_view typeName) override;
        void doPushObjectWithKey(std::string_view typeName, std::string_view key) override;
        void doSerializeValue(std::string_view key, bool value) override;
        void doSerializeValue(std::string_view key, std::string_view value) override;
        void doSerializeValue(std::string_view key, std::uint64_t value) override;
        void doSerializeValue(std::string_view key, std::uint32_t value) override;
        void doSerializeValue(std::string_view key, std::uint16_t value) override;
        void doSerializeValue(std::string_view key, std::uint8_t value) override;
        void doSerializeValue(std::string_view key, std::int64_t value) override;
        void doSerializeValue(std::string_view key, std::int32_t value) override;
        void doSerializeValue(std::string_view key, std::int16_t value) override;
        void doSerializeValue(std::string_view key, std::int8_t value) override;
        void doPopObject() override;
        void doPushArray(std::string_view key) override;
        void doPopArray() override;

      private:
        void ensureCurrentNodeIsMaterialized();
        void contextPush(YAML::Node node, bool isArray, std::string key = std::string());
        void contextPop();
        YAML::Node& getCurrentNode();
        void serializeScalarValue(std::string_view key, YAML::Node value);

      private:
        struct ContextEntry {
          YAML::Node m_node;
          bool m_isArray = false;
          bool m_isPendingArrayElement = false;
          bool m_isPendingValueArrayElement = false;
          bool m_isScalarArrayElement = false;
          std::string m_key;
          std::string m_pendingArrayElementTypeName;
        };

        YAML::Node m_root;
        std::vector<ContextEntry> m_yamlContext;
    };

} // namespace babelwires