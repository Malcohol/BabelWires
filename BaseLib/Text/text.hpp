/**
 * The Text class stores a string in a canonical representation.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>
#include <BaseLib/baseLibExport.hpp>

#include <string>
#include <string_view>

namespace babelwires {

    /// The canonical representation of text data.
    /// The class provides several methods for ingesting data held in a std::string_view. These ensure the ingested data
    /// was in exactly the expected form.
    /// The class provides several methods for egesting the data to a std::string. These ensure the egested data is in
    /// exactly the expected form.
    class BASELIB_API Text {
      public:
        Text() = default;
        Text(const Text&) = default;
        Text(Text&&) = default;
        /// This asserts that the string is indeed valid UTF-8.
        Text(std::u8string data);

        template <size_t N>
        Text(const char8_t (&data)[N])
            : Text(std::u8string(data, N - 1)) {}

        /// Return the number of textual characters (i.e. unicode code points) in the text
        std::size_t getTextLength() const { return m_textLength; }

        Text& operator=(const Text&) = default;
        Text& operator=(Text&&) = default;

        // These can fail
        static ResultT<Text> fromPrintableAscii(std::string_view ascii);
        static ResultT<Text> from7BitAscii(std::string_view ascii);
        static ResultT<Text> fromUtf8(std::string_view utf8);

        // These can assert
        static Text assertFromPrintableAscii(std::string_view ascii);
        static Text assertFrom7BitAscii(std::string_view ascii);
        static Text assertFromUtf8(std::string_view utf8);

        // Potentially lossy
        static Text tryFromPrintableAscii(std::string_view ascii);
        static Text tryFrom7BitAscii(std::string_view ascii);
        static Text tryFromUtf8(std::string_view utf8);

        // Does the stored data consist entirely of 7-bit ASCII characters?
        bool isPrintableAscii() const;

        // Does the stored data consist entirely of 7-bit ASCII characters?
        bool is7BitAscii() const;

        // Can fail
        ResultT<std::string> toPrintableAscii() const;
        ResultT<std::string> to7BitAscii() const;

        // Asserts if the stored data cannot be converted to 7-bit ASCII
        std::string assertToPrintableAscii() const;
        std::string assertTo7BitAscii() const;

        // Potentially lossy
        std::string tryToPrintableAscii() const;
        std::string tryTo7BitAscii() const;

        // Always succeeds (but should not be assumed to be a correctly encoded representation of the text as a
        // std::string on a system where UTF-8 is not the native encoding)
        std::string toUtf8() const;

        std::string serializeToString() const;
        static ResultT<Text> deserializeFromString(std::string_view str);

        auto operator<=>(const Text&) const = default;

        const std::u8string& getData() const { return m_data; }

      private:
        // Unlike the public constructor, this version doesn't perform any validation.
        Text(std::u8string checkedData, std::size_t textLength)
            : m_data(std::move(checkedData)), m_textLength(textLength) {}

        explicit Text(std::string_view checkedData, std::size_t textLength)
            : m_data(std::u8string(checkedData.begin(), checkedData.end())), m_textLength(textLength) {}

        /// The data is always in UTF-8 encoding.
        std::u8string m_data;

        /// The number of codepoints in the stored data.
        std::size_t m_textLength = 0;
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::Text> {
        inline std::size_t operator()(const babelwires::Text& text) const {
            return std::hash<std::u8string>()(text.getData());
        }
    };
} // namespace std
