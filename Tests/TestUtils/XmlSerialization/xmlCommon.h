/**
 * Functionality common to XML serialization and deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>
#include <string_view>

namespace babelwires {

    inline static constexpr std::string_view c_xmlMetadataPrefix = "meta:";
    inline static constexpr std::string_view c_xmlRuntimeTypeMetadataAttribute = "meta:type";
    inline static constexpr std::string_view c_xmlMetadataNamespaceAttribute = "xmlns:meta";
    inline static constexpr std::string_view c_xmlMetadataNamespace = "urn:babelwires:serialization-meta";

    // Assume the XML API copies string contents if necessary.
    struct toCStr {
        toCStr(std::string_view view)
            : m_str(view) {}
        operator const char*() { return m_str.c_str(); }
        std::string m_str;
    };

} // namespace babelwires