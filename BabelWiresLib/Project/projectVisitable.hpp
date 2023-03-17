/**
 * ProjectVisibable defines an interface for classes storing project data, allowing them to expose certain data
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
    struct ProjectVisitable;

    struct IdentifierVisitor {
        virtual ~IdentifierVisitor() = default;
        virtual void operator()(ShortId& identifier) = 0;
        virtual void operator()(MediumId& identifier) = 0;
        virtual void operator()(LongId& identifier) = 0;

        /// Allow the visitor to be applied to types in templates which may or may have identifier members.
        void operator()(...) {}

        /// Avoid a pitfall.
        void operator()(ProjectVisitable&) = delete;
        void operator()(const ProjectVisitable&) = delete;
    };
    
    class FilePath;
    using FilePathVisitor = std::function<void(FilePath&)>;

    /// An interface for classes storing project data, allowing them to expose certain data
    /// to important infrastructural visitors.
    struct ProjectVisitable {
        /// Call the visitor on all fields in the object.
        virtual void visitIdentifiers(IdentifierVisitor& visitor) = 0;

        /// Call the visitor on all FilePaths in the object.
        virtual void visitFilePaths(FilePathVisitor& visitor) = 0;
    };
}
