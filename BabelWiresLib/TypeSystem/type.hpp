/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <BabelWiresLib/Path/pathStep.hpp>

#include <optional>
#include <memory>

namespace babelwires {

    /// A type describes a valid set of values.
    /// Note that a value can be an instance of more than one type.
    /// A type is a subtypes of another type when all its values are valid
    /// values of the other.
    /// Information about subtype relationships is managed by the TypeSystem.
    class Type {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Type);

        virtual ~Type();

        /// Create a new Value representing a default instance of the type.
        virtual NewValueHolder createValue(const TypeSystem& typeSystem) const = 0;

        /// Is the value v an element of this type.
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const;

        /// Called by visitValue when a type contains children.
        /// Returning false aborts the visit and visitValue will return false.
        using ChildValueVisitor = std::function<bool(const TypeSystem& typeSystem, const TypeRef& childTypeRef,
                                                     const Value& childValue, const PathStep& stepToChild)>;

        /// Verify that the outer structure of the value conforms to this type, calling the visitor on each child value.
        /// This returns false if the value does not conform to this type, or if the visitor returns false.
        virtual bool visitValue(const TypeSystem& typeSystem, const Value& v,
                                ChildValueVisitor& visitor) const = 0;

        /// Get a TypeRef that describes this type.
        /// Registered types get an implementation of this method from the REGISTERED_TYPE macro.
        /// Types constructed by TypeConstructors must provide their own implementation.
        virtual TypeRef getTypeRef() const = 0;

        /// Return a short string which can be used in the UI to give a sense of the data this type handles.
        /// It is not used to impose any formal restrictions.
        /// In cases where the type has a unique and serializable class of values, the serializationType of that
        /// class is a good choice.
        virtual std::string getFlavour() const = 0;

        /// How are this type and other related by subtyping? A type may not know if it's related to
        /// other, so by default, this returns an empty optional. (If neither type know how they are
        /// related, then they are not related.)
        virtual std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const;

        /// Used for display to the user, so it does not have to be parsable or even comprehensive.
        virtual std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const = 0;

        /// Convenience function which returns a human-readable version of the type's TypeRef.
        std::string getName() const;

        /// An identifier which can be used to group related types together.
        using Tag = MediumId;

        /// Get the tags associated with this type.
        const std::vector<Tag>& getTags() const;

      protected:
        /// Only intended for use during subclass construction.
        void addTag(Tag tag);

      private:
        /// The tags associated with this type.
        std::vector<Tag> m_tags;
    };

} // namespace babelwires
