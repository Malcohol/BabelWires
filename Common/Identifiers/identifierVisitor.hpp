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
    struct DataVisitable;

    struct IdentifierVisitor {
        virtual ~IdentifierVisitor() = default;
        virtual void operator()(ShortId& identifier) = 0;
        virtual void operator()(MediumId& identifier) = 0;
        virtual void operator()(LongId& identifier) = 0;

        /// Allow the visitor to be applied to types in templates which may or may have identifier members.
        void operator()(...) {}

        // Applying a visitor to an object is _not_ the usual pattern for visiting it, but it's tempting to write
        // code of that form. The usual pattern is to call a visitIdentifiers method.
        // This catches misuses, at least for classes that derive from DataVisitable.
        void operator()(DataVisitable&) = delete;
        void operator()(const DataVisitable&) = delete;
    };
}
