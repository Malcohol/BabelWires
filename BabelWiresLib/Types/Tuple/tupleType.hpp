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
    /// Tuple types are primarily added to allow for complex maps, so for now do not use CompoundType.
    /// Also: The parameter types are expected to have EditableValues.
    class TupleType : public Type {
      public:
        using ComponentTypes = std::vector<TypeRef>;

        TupleType(ComponentTypes components);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        const ComponentTypes& getComponentTypes() const;

        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        ComponentTypes m_componentTypes;
    };
} // namespace babelwires
