/**
 * TupleType
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// TupleType (a.k.a. Product Type).
    class TupleType : public Type {
      public:
        using ParameterTypes = std::vector<TypeRef>;

        TupleType(ParameterTypes parameters);

        /// Always creates a value of the first summand.
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        const ParameterTypes& getParameterTypes() const;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        ParameterTypes m_parameterTypes;
    };
} // namespace babelwires
