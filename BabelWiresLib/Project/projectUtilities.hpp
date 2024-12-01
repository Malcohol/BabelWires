/**
 * Utilities useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Path/path.hpp>

namespace babelwires {
    struct UiPosition;
    struct ProjectData;
    class Node;
    class Project;

    namespace projectUtilities {
        void translate(const UiPosition& offset, ProjectData& dataInOut);

        /// Get all values in the project whose structure derives via a connection from the value at the given element and path.
        std::vector<std::tuple<NodeId, Path>> getDerivedValues(const Project& project, NodeId nodeId, const Path& pathToValue);

        /// Get all values in the project whose structure directly or indirectly derives from the value at the given element and path.
        /// The original values are not included in the array.
        std::vector<std::tuple<NodeId, Path>> getAllDerivedValues(const Project& project, NodeId nodeId, const Path& pathToValue);
    }
} // namespace babelwires