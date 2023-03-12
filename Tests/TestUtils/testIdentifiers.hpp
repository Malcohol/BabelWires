#pragma once

#include <Common/Identifiers/identifier.hpp>


namespace testUtils {
    /// Get an identifier with the given discriminator.
    /// Convenience function for tests which don't require the id to be actually registered.
    babelwires::ShortId getTestRegisteredIdentifier(std::string_view name, unsigned int discriminator = 1);

    /// Get a long identifier with the given discriminator.
    /// Convenience function for tests which don't require the id to be actually registered.
    babelwires::LongId getTestRegisteredLongIdentifier(std::string_view name, unsigned int discriminator = 1);
}
