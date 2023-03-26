/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intType.hpp>

#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

std::unique_ptr<babelwires::Value> babelwires::IntType::createValue() const {
    return std::make_unique<IntValue>();
}
