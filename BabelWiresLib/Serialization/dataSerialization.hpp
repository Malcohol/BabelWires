/**
 * Functions for saving and loading data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <filesystem>
#include <istream>
#include <memory>
#include <ostream>

namespace babelwires {
    struct UserLogger;
    struct ProjectContext;

    /// Isolates the codebase from the choice of format.
    /// BUNDLE should derive from DataBundle are defines the Data payload.
    template <typename BUNDLE> class DataSerialization {
      public:
        using Data = typename BUNDLE::Data;

        static Data loadFromFile(const std::filesystem::path& pathToFile, const ProjectContext& context,
                                 UserLogger& userLogger);

        /// Throws a FileIoException on failure.
        static void saveToFile(const std::filesystem::path& pathToFile, Data data);

        /// Load the data from the given string, resolving the data in the given context and pathToFile.
        /// pathToFile may be empty, in which case only absolute file paths in the data can be resolved.
        /// That might happen if data is being pasted into an unsaved project. Fortunately, this probably means that the
        /// absolute paths originate in the current filesystem.
        static Data loadFromString(const std::string& string, const ProjectContext& context,
                                   const std::filesystem::path& pathToFile, UserLogger& userLogger);

        /// Serialize the data and return it as a string.
        /// pathToFile is used to store relative versions of file paths.
        /// pathToFile may be empty, in which case only absolute file paths in the data will be stored.
        /// That might happen if data is being copied from an unsaved project.
        static std::string saveToString(const std::filesystem::path& pathToFile, Data data);

      private:
        /// Load the data from is, resolving the data in the given context and pathToFile.
        static Data loadFromStream(std::istream& is, const ProjectContext& context,
                                   const std::filesystem::path& pathToFile, UserLogger& userLogger);

        static void saveToStream(std::ostream& os, const std::filesystem::path& pathToFile, Data data);
    };
} // namespace babelwires

#include <BabelWiresLib/Serialization/dataSerialization_inl.hpp>
