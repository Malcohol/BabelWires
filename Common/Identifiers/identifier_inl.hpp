/**
 * An identifier uniquely identifies an object in some local context (e.g. a field in a record) and globally.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/exceptions.hpp>

#include <charconv>

template <unsigned int NUM_BLOCKS>
typename babelwires::IdentifierBase<NUM_BLOCKS>::Discriminator
babelwires::IdentifierBase<NUM_BLOCKS>::getDiscriminator() const {
    return m_data.m_discriminator;
}

template <unsigned int NUM_BLOCKS>
void babelwires::IdentifierBase<NUM_BLOCKS>::setDiscriminator(Discriminator index) {
    m_data.m_discriminator = index;
}

template <unsigned int NUM_BLOCKS>
void babelwires::IdentifierBase<NUM_BLOCKS>::copyDiscriminatorTo(IdentifierBase& other) const {
    if (other.getDiscriminator() == 0) {
        other.setDiscriminator(getDiscriminator());
    }
}

template <unsigned int NUM_BLOCKS>
template <size_t... INSEQ>
auto babelwires::IdentifierBase<NUM_BLOCKS>::getTupleOfCodesFromIndexSequence(std::index_sequence<INSEQ...>) const {
    return std::make_tuple(getDataAsCode<INSEQ>()...);
}

template <unsigned int NUM_BLOCKS>
template <size_t... INSEQ>
std::size_t babelwires::IdentifierBase<NUM_BLOCKS>::getHashFromIndexSequence(std::index_sequence<INSEQ...>) const {
    return hash::mixtureOf(getDataAsCode<INSEQ>()...);
}

template <unsigned int NUM_BLOCKS> babelwires::IdentifierBase<NUM_BLOCKS>::IdentifierBase(std::string_view str) {
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

template <unsigned int NUM_BLOCKS>
babelwires::IdentifierBase<NUM_BLOCKS>
babelwires::IdentifierBase<NUM_BLOCKS>::deserializeFromString(std::string_view str) {
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
    if (discriminator > c_maxDiscriminator) {
        throw ParseException() << "The string \"" << str << "\" has a discriminator which is too large";
    }
    IdentifierBase<NUM_BLOCKS> f(str);
    f.setDiscriminator(discriminator);
    return f;
}

template <unsigned int NUM_BLOCKS>
void babelwires::IdentifierBase<NUM_BLOCKS>::writeTextToStream(std::ostream& os) const {
    for (int i = N - 1; i >= 0; --i) {
        if (m_data.m_chars[i] == '\0') {
            break;
        }
        os << m_data.m_chars[i];
    }
}

template <unsigned int NUM_BLOCKS> std::string babelwires::IdentifierBase<NUM_BLOCKS>::toString() const {
    std::ostringstream os;
    writeTextToStream(os);
    return os.str();
}

template <unsigned int NUM_BLOCKS> std::string babelwires::IdentifierBase<NUM_BLOCKS>::serializeToString() const {
    std::ostringstream os;
    writeTextToStream(os);
    if (m_data.m_discriminator > 0) {
        os << s_discriminatorDelimiter << static_cast<int>(m_data.m_discriminator);
    }
    return os.str();
}

template <unsigned int NUM_BLOCKS> bool babelwires::IdentifierBase<NUM_BLOCKS>::validate(const char* chars, size_t n) {
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
        // Restricting to typical identifier rules makes it easier to parse expressions, e.g. paths.
        // Also, it guarantees that the high-bit is available for tagging the path step union.
        const char c = chars[i];
        if (!std::isalnum(c) && (c != '_')) {
            return false;
        }
    }
    return true;
}
