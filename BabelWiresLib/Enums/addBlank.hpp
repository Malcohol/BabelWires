/**
 * A type constructor which adds a blank value to an existing enum
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/Enums/enum.hpp>

namespace babelwires {
    /// A unary type constructor which adds a blank value to an existing enum.
    /// This is useful in map targets to indicate when the map should exclude an entry, rather than select a mapped value.
    class AddBlank : public TypeConstructor {
      public:
        TYPE_CONSTRUCTOR("AddBlank", "{0} + _", "bd5af7a5-4a75-4807-a3d8-93851e1a7d00", 1);

        /// The Dummy value added to enums. 
        /// Note that the blank value is only permitted at the end of the enum.
        static Identifier getBlankValue();

        unsigned int getArity() const override;

        std::unique_ptr<Type> constructType(TypeRef newTypeRef, const std::vector<const Type*>& arguments) const override;
    };

    namespace detail {
      /// The Enum class created by the AddBlank type constructor.
      class EnumWithBlank : public Enum {
        public:
          EnumWithBlank(TypeRef typeRef, const Enum& srcEnum);
        
          TypeRef getTypeRef() const override;

          /// Doesn't add a blank if one is already there.
          static EnumValues ensureBlankValue(const EnumValues& srcValues);

        public:
          TypeRef m_typeRef;
      };
    }
} // namespace babelwires
