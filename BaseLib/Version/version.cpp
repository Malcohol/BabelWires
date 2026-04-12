/**
 * A Version struct providing the usual major, minor, patch version numbers.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BaseLib/Version/version.hpp>
#include <BaseLib/Version/codebaseVersion_gen.hpp>

#include <BaseLib/Result/resultDSL.hpp>

#include <charconv>
#include <regex>

namespace {
    // Semantic Versioning 2.0.0 regex from https://semver.org/spec/v2.0.0.html
    const std::regex s_semVerRegex(
        R"(^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)");

    babelwires::ResultT<std::uint16_t> parseVersionComponent(const std::string& text) {
        std::uint16_t out = 0;
        const std::from_chars_result parseResult = std::from_chars(text.data(), text.data() + text.size(), out);
        if (parseResult.ec == std::errc::result_out_of_range) {
            return babelwires::Error() << "Version component \"" << text << "\" is too large";
        }
        if ((parseResult.ec != std::errc()) || (parseResult.ptr != (text.data() + text.size()))) {
            return babelwires::Error() << "Failed to parse version component \"" << text << "\"";
        }
        return out;
    }
} // namespace

const babelwires::Version& babelwires::Version::getCodebaseVersion() {
    static const Version s_codebaseVersion = {
        detail::c_codebaseVersionMajor,
        detail::c_codebaseVersionMinor,
        detail::c_codebaseVersionPatch,
    };

    return s_codebaseVersion;
}

bool babelwires::Version::satisfies(const Version& requiredVersion) const {
    if ((m_major == 0) || (requiredVersion.m_major == 0)) {
        return (m_major == requiredVersion.m_major) && (m_minor == requiredVersion.m_minor) && (m_patch == requiredVersion.m_patch);
    }

    if (m_major != requiredVersion.m_major) {
        return false;
    }

    if (m_minor != requiredVersion.m_minor) {
        return m_minor > requiredVersion.m_minor;
    }

    return m_patch >= requiredVersion.m_patch;
}

std::string babelwires::Version::toString() const {
    return std::to_string(m_major) + "." + std::to_string(m_minor) + "." + std::to_string(m_patch);
}

std::string babelwires::Version::serializeToString() const {
    return toString();
}

babelwires::ResultT<babelwires::Version> babelwires::Version::deserializeFromString(std::string_view versionText) {
    std::match_results<std::string_view::const_iterator> match;
    if (!std::regex_match(versionText.begin(), versionText.end(), match, s_semVerRegex)) {
        return Error() << "Failed to parse version string \"" << versionText << "\"";
    }

    ASSIGN_OR_ERROR(std::uint16_t major, parseVersionComponent(match[1].str()));
    ASSIGN_OR_ERROR(std::uint16_t minor, parseVersionComponent(match[2].str()));
    ASSIGN_OR_ERROR(std::uint16_t patch, parseVersionComponent(match[3].str()));

    return Version{major, minor, patch};
}