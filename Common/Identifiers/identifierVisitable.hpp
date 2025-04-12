/**
 * IdentifierVisitable defines an interface for classes storing identifiers, allowing them to be exposed
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

#include <functional>

namespace babelwires {
    struct IdentifierVisitable;

    struct IdentifierVisitor {
        virtual ~IdentifierVisitor() = default;
        virtual void operator()(ShortId& identifier) = 0;
        virtual void operator()(MediumId& identifier) = 0;
        virtual void operator()(LongId& identifier) = 0;

        /// Allow the visitor to be applied to types in templates which may or may have identifier members.
        void operator()(...) {}

        /// Catch a pitfall at compile time.
        void operator()(IdentifierVisitable&) = delete;
        void operator()(const IdentifierVisitable&) = delete;
    };
    
    /// IdentifierVisitable defines an interface for classes storing identifiers, allowing them to be exposed
    /// to important infrastructural visitors.
    // TODO This is mostly used to enforce an interface rather than provide an abstraction, so maybe a concept would 
    // be more appropriate.
    struct IdentifierVisitable {
        /// Call the visitor on all identifiers in the object.
        virtual void visitIdentifiers(IdentifierVisitor& visitor) = 0;
    };
}
