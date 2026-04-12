/**
 * A Version struct providing the usual major, minor, patch version numbers.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>
#include <BaseLib/baseLibExport.hpp>

#include <string_view>

#include <cstdint>

namespace babelwires {

    /// A Version struct providing the usual major, minor and patch version numbers.
    /// The version has a dual role:
    /// * It represents the application version and communicates expectations about functionality to the user.
    /// * From the perspective of plugins, the version value is intended to approximately follow semantic versioning
    ///   (see https://semver.org/) and communicates expectations about API stability and compatibility.
    struct BASELIB_API Version {
        /// A major version change always indicates incompatibility.
        /// However, a major version of zero is a special case and indicates that the API is not yet stable.
        std::uint16_t major;
        /// A minor version change indicates backward-compatible functionality additions.
        std::uint16_t minor;
        /// A patch version change indicates backward-compatible bug fixes.
        std::uint16_t patch;

        /// Return the version of this project.
        /// This is set from values in the (auto-generated) file projectVersion_gen.hpp.
        /// The original source of the version values is the root CMakeLists.txt file.
        static const Version& getProjectVersion();

        /// Return true if this version satisfies the given required version.
        /// Specifically, an API with this version can be consumed where someone requires the given requiredVersion.
        ///
        /// Compatibility rules:
        /// * For major >= 1, this follows semantic versioning compatibility:
        ///   same major and this version >= requiredVersion.
        /// * For major == 0, compatibility is treated conservatively and requires exact match.
        bool satisfies(const Version& requiredVersion) const;

        /// Return a string representation of this version, in the format "major.minor.patch".
        std::string toString() const;

        /// Serialize this version to a string in the format "major.minor.patch".
        std::string serializeToString() const;

        /// Parse a serialized version string. For future-proofing, any valid SemVer 2.0.0 string is accepted, but only
        /// the major, minor and patch components are stored in the Version struct.
        static ResultT<Version> deserializeFromString(std::string_view versionText);
    };

} // namespace babelwires
