/**
 * An identifier uniquely identifies an object in some local context (e.g. a field in a record) and globally.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Identifiers/identifier.hpp"

#include "Common/exceptions.hpp"

#include <charconv>

babelwires::Identifier::Identifier(std::string_view str) {
    const size_t len = str.size();
    assert((len > 0) && "Identifiers may not be empty");
    assert((len <= N) && "str is too long.");
    assert(validate(str.data(), str.size()) && "The identifier is invalid");
    // TODO Big endian.
    // std::copy(str.begin(), str.end(), m_data.m_chars);
    // std::fill(m_data.m_chars + len, m_data.m_chars + N, 0);
    std::reverse_copy(str.begin(), str.end(), m_data.m_chars + N - len);
    std::fill(m_data.m_chars, m_data.m_chars + N - len, 0);
}

babelwires::Identifier babelwires::Identifier::deserializeFromString(std::string_view str) {
    Discriminator discriminator = 0;
    std::size_t idEnd = str.find(s_discriminatorDelimiter);
    if (idEnd != std::string_view::npos) {
        std::from_chars_result result = std::from_chars(str.data() + idEnd + 1, str.data() + str.size(), discriminator);
        if (result.ec != std::errc()) {
            throw ParseException() << "The disciminator part of \"" << str << "\" could not be parsed";
        }
        str.remove_suffix(str.size() - idEnd);
    } else {
        idEnd = str.size();
    }
    if (idEnd > N) {
        throw ParseException() << "The string \"" << str << "\" is too long for an identifier";
    }
    if (!validate(str.data(), str.size())) {
        throw ParseException() << "The string \"" << str << "\" is not a valid identifier";
    }
    if (discriminator > c_maxDiscriminator)
    {
        throw ParseException() << "The string \"" << str << "\" has a discriminator which is too large";
    }
    Identifier f(str);
    f.setDiscriminator(discriminator);
    return f;
}

void babelwires::Identifier::writeToStream(std::ostream& os) const {
    for (int i = N - 1; i >= 0; --i) {
        if (m_data.m_chars[i] == '\0') {
            break;
        }
        os << m_data.m_chars[i];
    }
    if (m_data.m_discriminator > 0) {
        os << s_discriminatorDelimiter << static_cast<int>(m_data.m_discriminator);
    }
}

/// Return a human-readable version of the identifier.
std::string babelwires::Identifier::serializeToString() const {
    std::ostringstream oss;
    writeToStream(oss);
    return oss.str();
}

bool babelwires::Identifier::validate(const char* chars, size_t n) {
    assert(n >= 0);
    assert(n <= N);
    if ((chars[0] == '\0') || (n == 0)) {
        // Must be non-trivial.
        return false;
    }
    if (std::isdigit(chars[0])) {
        // We need to distinguish identifiers from array indices when parsing paths.
        return false;
    }
    for (int i = 0; i < n; ++i) {
        // Path and discriminator delimiters would make paths impossible to parse.
        // We rely on the high-bit for tagging the path step union.
        const char c = chars[i];
        if ((c == s_discriminatorDelimiter) || (c == s_pathDelimiter) || !std::isgraph(c)) {
            assert((c <= 127) && "Assumption that std::isgraph allows only 7-bit ASCII is wrong.");
            return false;
        }
    }
    return true;
}

void babelwires::Identifier::copyDiscriminatorToInternal(const Identifier& other) const {
    assert(other.getDiscriminator() == 0);
    other.setDiscriminator(getDiscriminator());
}
