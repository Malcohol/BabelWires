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

        // Catch a potential pitfall at compile time.
        // Note: These could call visitIdentifiers.
        void operator()(DataVisitable&) = delete;
        void operator()(const DataVisitable&) = delete;
    };
}
