/**
 * A Version struct providing the usual major, minor, patch version numbers.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BaseLib/Version/version.hpp>
#include <BaseLib/Version/projectVersion_gen.hpp>

const babelwires::Version& babelwires::Version::projectVersion() {
    static const Version s_projectVersion = {
        detail::c_projectVersionMajor,
        detail::c_projectVersionMinor,
        detail::c_projectVersionPatch,
    };

    return s_projectVersion;
}

bool babelwires::Version::satisfies(const Version& requiredVersion) const {
    if ((major == 0) || (requiredVersion.major == 0)) {
        return (major == requiredVersion.major) && (minor == requiredVersion.minor) && (patch == requiredVersion.patch);
    }

    if (major != requiredVersion.major) {
        return false;
    }

    if (minor != requiredVersion.minor) {
        return minor > requiredVersion.minor;
    }

    return patch >= requiredVersion.patch;
}
