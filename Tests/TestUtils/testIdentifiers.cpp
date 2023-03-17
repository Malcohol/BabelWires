#include <Tests/TestUtils/testIdentifiers.hpp>

babelwires::ShortId testUtils::getTestRegisteredIdentifier(std::string_view name, unsigned int discriminator) {
    babelwires::ShortId id = name;
    id.setDiscriminator(discriminator);
    return id;
}

babelwires::MediumId testUtils::getTestRegisteredMediumIdentifier(std::string_view name, unsigned int discriminator) {
    babelwires::MediumId id = name;
    id.setDiscriminator(discriminator);
    return id;
}

babelwires::LongId testUtils::getTestRegisteredLongIdentifier(std::string_view name, unsigned int discriminator) {
    babelwires::LongId id = name;
    id.setDiscriminator(discriminator);
    return id;
}
