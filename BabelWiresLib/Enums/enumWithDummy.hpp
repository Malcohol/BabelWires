/**
 * A type constructor which adds a dummy value to an existing enum
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Enums/enum.hpp>

#include <vector>
#include <unordered_map>

namespace babelwires {

    /// An Enum with a dummy value.
    class EnumWithDummy : public Enum {
      public:
        EnumWithDummy(TypeRef typeRef, const Enum& srcEnum);

        /// The Dummy value is only permitted at the end of the enum.
        static Identifier getDummyValue();
      
        TypeRef getTypeRef() const override;

      protected:
        /// Doesn't add a dummy if one is already there.
        static EnumValues ensureDummyValue(const EnumValues& srcValues);

      public:
        TypeRef m_typeRef;
    };

    /// A unary type constructor which adds a dummy value to an existing enum.
    class AddDummy : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("AddDummy", "({0}+\u22a5)", "bd5af7a5-4a75-4807-a3d8-93851e1a7d00", 1);

        unsigned int getArity() const override;

        std::unique_ptr<Type> constructType(TypeRef newTypeRef, const std::vector<const Type*>& arguments) const override;
    };
} // namespace babelwires
