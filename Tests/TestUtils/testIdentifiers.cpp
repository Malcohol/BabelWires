#include <Tests/TestUtils/testIdentifiers.hpp>

babelwires::ShortId testUtils::getTestRegisteredIdentifier(std::string_view name, unsigned int discriminator) {
    babelwires::ShortId id = name;
    id.setDiscriminator(discriminator);
    return id;
}

babelwires::LongIdentifier testUtils::getTestRegisteredLongIdentifier(std::string_view name, unsigned int discriminator) {
    babelwires::LongIdentifier id = name;
    id.setDiscriminator(discriminator);
    return id;
}
