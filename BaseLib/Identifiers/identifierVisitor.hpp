/**
 * The IdentifierVisitor is an interface for objects which need to visit all the identifiers in some object.
 *
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Identifiers/identifier.hpp>

#include <functional>

namespace babelwires {
    struct IdentifierVisitor;

    /// The standard pattern for an object being visitable is that it has a visitIdentifiers
    /// method that takes a visitor parameter.
    template<typename T> concept IdentifierVisitable = requires(T t, IdentifierVisitor& visitor) {
        t.visitIdentifiers(visitor);
    };

    /// The IdentifierVisitor is an interface for objects which need to visit all the identifiers in some object.
    /// The use-cases are for preparing identifiers when they an object is being moved from one context to another.
    /// For example, when data is deserialized from a file, the discriminators may not match those of the current system.
    /// Because the job is to adjust the identifiers, the visitor expects non-const access to the object it visits.
    struct IdentifierVisitor {
        virtual ~IdentifierVisitor() = default;
        virtual void operator()(ShortId& identifier) = 0;
        virtual void operator()(MediumId& identifier) = 0;
        virtual void operator()(LongId& identifier) = 0;

        /// Allow the visitor to be applied to types in templates which may or may have identifier members.
        void operator()(...) {}

        // Applying a visitor to an object is _not_ the usual pattern for visiting it. However, I've mistakenly
        // written code of that form which didn't get caught because of the ... overload.
        // The following deletions should help catch these mistakes.

        template<typename T> requires IdentifierVisitable<T>
        void operator()(T&) = delete;

        template<typename T> requires IdentifierVisitable<T>
        void operator()(const T&) = delete;
    };
}
