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
#include <filesystem>

namespace babelwires {
    class UserLogger;
}

namespace babelwires {

    class ProjectContext;

    /// Isolates the codebase from the choice of format and other implementation details of serialization.
    namespace ProjectSerialization {
        ProjectData loadFromFile(const std::filesystem::path& pathToProjectFile, const ProjectContext& context, UserLogger& userLogger);

        /// Throws a FileIoException on failure.
        void saveToFile(const std::filesystem::path& pathToProjectFile, ProjectData projectData);

        /// Load the projectData from the given string, resolving the data in the given context and pathToProjectFile.
        /// pathToProjectFile may be empty, in which case only absolute file paths in the data can be resolved.
        /// That might happen if data is being pasted into an unsaved project. Fortunately, this probably means that the
        /// absolute paths originate in the current filesystem.
        ProjectData loadFromString(const std::string& string, const ProjectContext& context, const std::filesystem::path& pathToProjectFile, UserLogger& userLogger);

        /// Serialize the projectData and return it as a string.
        /// pathToProjectFile is used to store relative versions of file paths.
        /// pathToProjectFile may be empty, in which case only absolute file paths in the data will be stored.
        /// That might happen if data is being copied from an unsaved project.
        std::string saveToString(const std::filesystem::path& pathToProjectFile, ProjectData projectData);

        namespace internal {
            /// Load the projectData from is, resolving the data in the given context and pathToProjectFile.
            ProjectData loadFromStream(std::istream& is, const ProjectContext& context, const std::filesystem::path& pathToProjectFile, UserLogger& userLogger);

            void saveToStream(std::ostream& os, const std::filesystem::path& pathToProjectFile, ProjectData projectData);
        }
    } // namespace ProjectSerialization
} // namespace babelwires
