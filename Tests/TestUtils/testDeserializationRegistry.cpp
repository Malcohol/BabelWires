#include "Tests/TestUtils/testDeserializationRegistry.hpp"

void testUtils::TestDeserializationRegistry::registerEntry(std::string_view typeName, const Entry* entry) {
    m_nameToEntry.insert(std::make_pair(std::string(typeName), entry));
}

const babelwires::DeserializationRegistry::Entry*
testUtils::TestDeserializationRegistry::findEntry(std::string_view typeName) const {
    const auto it = m_nameToEntry.find(std::string(typeName));

    if (it != m_nameToEntry.end()) {
        return it->second;
    }

    return babelwires::AutomaticDeserializationRegistry::findEntry(typeName);
}
