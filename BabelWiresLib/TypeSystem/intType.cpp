#include <BabelWiresLib/TypeSystem/intType.hpp>

#include <BabelWiresLib/TypeSystem/intValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::LongIdentifier babelwires::IntType::getThisIdentifier() {
    return REGISTERED_LONGID("int", "integer", "90ed4c0c-2fa1-4373-9b67-e711358af824");
}

babelwires::IntType::IntType()
    : Type(getThisIdentifier(), 1) {}

std::unique_ptr<babelwires::Value> babelwires::IntType::createValue() const {
    return std::make_unique<IntValue>();
}
