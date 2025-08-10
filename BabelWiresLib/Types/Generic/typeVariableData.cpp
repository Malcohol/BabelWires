/**
 * TypeVariableData carries the data that identifies an instance of a type variable.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

#include <Common/Utilities/unicodeUtils.hpp>

#include <sstream>
#include <cassert>

std::optional<babelwires::TypeVariableData> babelwires::TypeVariableData::isTypeVariable(const TypeRef& typeRef) {
    struct Visitor {
        std::optional<TypeVariableData> operator()(std::monostate) {
            return {};
        }
        std::optional<TypeVariableData> operator()(const RegisteredTypeId& typeId) { 
            // Reasonable assumption: no one would register a type variable type.
            return {};
        }
        std::optional<TypeVariableData> operator()(const TypeConstructorId& constructorId, const TypeConstructorArguments& constructorArguments) {
            if (constructorId == TypeVariableTypeConstructor::getThisIdentifier()) {
                return TypeVariableTypeConstructor::extractValueArguments(constructorArguments.m_valueArguments);
            } else {
                return {};
            }
        }
    } visitor;
    return typeRef.visit<Visitor, std::optional<TypeVariableData>>(visitor);
}

std::string babelwires::TypeVariableData::toString() const {
    std::ostringstream os;
    assert(m_typeVariableIndex < c_maxNumTypeVariables && "VariableIndex is larger than the maximum allowed number of type variables");
    const char letter = (('T' - 'A' + m_typeVariableIndex) % 26) + 'A';
    os.put(letter);
    if (m_numGenericTypeLevels > 0) {
        writeUnicodeSuperscript(os, m_numGenericTypeLevels);
    }
    return os.str();
}
