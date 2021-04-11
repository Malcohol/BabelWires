/**
 * Functions for saving and loading a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/projectData.hpp"

#include <istream>
#include <memory>
#include <ostream>

namespace babelwires {
    class UserLogger;
}

namespace babelwires {

    class ProjectContext;

    /// Isolates the codebase from the choice of format and other implementation details of serialization.
    namespace ProjectSerialization {
        ProjectData loadFromStream(std::istream& is, const ProjectContext& context, UserLogger& userLogger);

        void saveToStream(std::ostream& os, ProjectData projectData);

        ProjectData loadFromFile(const char* filePath, const ProjectContext& context, UserLogger& userLogger);

        ProjectData loadFromString(const std::string& string, const ProjectContext& context, UserLogger& userLogger);

        /// Throws a FileIoException on failure.
        void saveToFile(const char* filePath, ProjectData projectData);

        std::string saveToString(ProjectData projectData);
    } // namespace ProjectSerialization

} // namespace babelwires
