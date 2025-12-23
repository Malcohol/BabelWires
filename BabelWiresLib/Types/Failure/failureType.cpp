/**
 * A type which can stand in when a type or element fails to resolve in the current system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Failure/failureType.hpp>

#include <BabelWiresLib/Types/Failure/failureValue.hpp>

babelwires::FailureType::FailureType(std::string message)
    : m_message(std::move(message)) {}

babelwires::NewValueHolder babelwires::FailureType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<FailureValue>();
}

bool babelwires::FailureType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    return v.as<FailureValue>();
}

std::string babelwires::FailureType::getFlavour() const {
    return "fail";
}
    
std::optional<babelwires::SubtypeOrder> babelwires::FailureType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                       const Type& other) const {
    return SubtypeOrder::IsDisjoint;
}

std::string babelwires::FailureType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    assert(v->as<FailureValue>() && "Value is not a FailureValue");
    return m_message;
}
