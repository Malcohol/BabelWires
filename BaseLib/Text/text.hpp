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
    /// The data is always stored in UTF-8 encoding.
    /// The class provides several methods for ingesting data from a std::string_view. These ensure the ingested data
    /// was in exactly the expected form and that the stored data is valid UTF-8.
    /// The class provides several methods for egesting the data to a std::string. These ensure the egested data is in
    /// exactly the expected form. 
    class BASELIB_API Text {
      public:
        Text() = default;
        Text(const Text&) = default;
        Text(Text&&) = default;
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

        // Always succeeds
        std::string_view toUtf8() const;

      private:
        explicit Text(std::string data)
            : m_data(std::move(data)) {}

        /// The data is always in UTF-8 encoding.
        std::string m_data;
    };

} // namespace babelwires
