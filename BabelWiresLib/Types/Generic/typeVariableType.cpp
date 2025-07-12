/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::TypeVariableType::TypeVariableType() = default;

std::string babelwires::TypeVariableType::getFlavour() const {
    return "variable";
}

