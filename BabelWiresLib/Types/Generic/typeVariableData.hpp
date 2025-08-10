/**
 * TypeVariableData carries the data that identifies an instance of a type variable.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>
#include <optional>

namespace babelwires {
    class TypeRef;

    /// TypeVariableData carries the data that identifies an instance of a type variable.
    struct TypeVariableData {
        /// The index in the generic type of which this is a variable.
        unsigned int m_typeVariableIndex;

        /// The number of generic types to traverse upwards to find the generic type of which this is a variable.
        unsigned int m_numGenericTypeLevels  = 0;

        /// Convenience method
        static std::optional<TypeVariableData> isTypeVariable(const TypeRef& typeRef);

        static constexpr unsigned int c_maxNumTypeVariables = 26;

        std::string toString() const;
    };

} // namespace babelwires
