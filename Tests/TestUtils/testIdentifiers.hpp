#include "Common/Identifiers/identifier.hpp"


namespace testUtils {
    /// Get an identifier with the given discriminator.
    /// Convenience function which avoids the necessity of registering the id.
    babelwires::Identifier getTestRegisteredIdentifier(std::string_view name, unsigned int discriminator = 1) {
        babelwires::Identifier id = name;
        id.setDiscriminator(discriminator);
        return id;
    }

    /// Get a long identifier with the given discriminator.
    /// Convenience function which avoids the necessity of registering the id.
    babelwires::LongIdentifier getTestRegisteredLongIdentifier(std::string_view name, unsigned int discriminator = 1) {
        babelwires::Identifier id = name;
        id.setDiscriminator(discriminator);
        return id;
    }
}
