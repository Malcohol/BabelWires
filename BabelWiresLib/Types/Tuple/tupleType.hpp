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
        using ComponentTypesExps = std::vector<TypeExp>;

        TupleType(TypeExp typeExp, const TypeSystem& typeSystem, ComponentTypesExps components);

        using ComponentTypes = std::vector<TypePtr>;

        TupleType(TypeExp typeExp, ComponentTypes components);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        std::string getFlavour() const override;

        const ComponentTypes& getComponentTypes() const;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        ComponentTypes m_componentTypes;
    };
} // namespace babelwires
