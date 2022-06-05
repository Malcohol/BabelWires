/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Registry/registry.hpp>
#include <Common/Identifiers/identifier.hpp>

#include <optional>

namespace babelwires {

    class Type : public RegistryEntry {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Type);
        Type(LongIdentifier identifier, VersionNumber version, std::optional<LongIdentifier> parentTypeId = {});
        
        std::optional<LongIdentifier> getParentTypeId() const;

        virtual std::unique_ptr<Value> createValue() const = 0;

        /// This is called by the type system when children are registered.
        void addChild(const Type* child);

        /// This is called by the type system when this type is registered.
        void setParent(const Type* parent);

        const Type* getParent() const;

        /// Get the direct subtypes of this type.
        const std::vector<const Type*>& getChildren() const;

      protected:
        /// Confirm that the parent when resolved in the type system is the expected parent.
        /// The default implementation asserts.
        virtual bool verifyParent(const Type& parentType) const;

      private:
        std::optional<LongIdentifier> m_parentTypeId;

        // Set after registration
        const Type* m_parent = nullptr;
        std::vector<const Type*> m_children;
    };

}
