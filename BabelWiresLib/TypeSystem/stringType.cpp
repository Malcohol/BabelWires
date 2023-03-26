/**
 * The type of strings.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/stringType.hpp>

#include <BabelWiresLib/TypeSystem/stringValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

std::unique_ptr<babelwires::Value> babelwires::StringType::createValue() const {
    return std::make_unique<StringValue>();
}
