/**
 * Functions for saving and loading a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Serialization/projectSerialization.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"
#include "BabelWires/Serialization/projectBundle.hpp"

#include "Common/IO/outFileStream.hpp"
#include "Common/Serialization/XML/xmlDeserializer.hpp"
#include "Common/Serialization/XML/xmlSerializer.hpp"
#include "Common/exceptions.hpp"

#include <fstream>

babelwires::ProjectData babelwires::ProjectSerialization::internal::loadFromStream(std::istream& is,
                                                                         const ProjectContext& context,
                                                                         const std::filesystem::path& pathToProjectFile,
                                                                         UserLogger& userLogger) {
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    XmlDeserializer deserializer(str, context.m_deserializationReg, userLogger);
    try {
        auto projectBundle = deserializer.deserializeObject<ProjectBundle>(ProjectBundle::serializationType);
        assert(projectBundle);
        deserializer.finalize();
        return std::move(*projectBundle).resolveAgainstCurrentContext(context, pathToProjectFile, userLogger);
    } catch (ParseException& e) {
        deserializer.addContextDescription(e);
        throw;
    }
}

babelwires::ProjectData babelwires::ProjectSerialization::loadFromFile(const std::filesystem::path& pathToProjectFile,
                                                                       const ProjectContext& context,
                                                                       UserLogger& userLogger) {
    std::ifstream is(pathToProjectFile);
    try {
        return internal::loadFromStream(is, context, pathToProjectFile, userLogger);
    } catch (ParseException& e) {
        e << " when reading the file \"" << pathToProjectFile << "\"";
        throw;
    }
}

babelwires::ProjectData babelwires::ProjectSerialization::loadFromString(const std::string& string,
                                                                         const ProjectContext& context,
                                                                         const std::filesystem::path& pathToProjectFile,
                                                                         UserLogger& userLogger) {
    std::istringstream is(string);
    return internal::loadFromStream(is, context, pathToProjectFile, userLogger);
}

void babelwires::ProjectSerialization::internal::saveToStream(std::ostream& os, const std::filesystem::path& pathToProjectFile, ProjectData projectData) {
    XmlSerializer serializer;
    ProjectBundle bundle(pathToProjectFile, std::move(projectData));
    serializer.serializeObject(bundle);
    serializer.write(os);
}

void babelwires::ProjectSerialization::saveToFile(const std::filesystem::path& pathToProjectFile, ProjectData projectData) {
    try {
        OutFileStream os(pathToProjectFile);
        internal::saveToStream(os, pathToProjectFile, std::move(projectData));
        os.close();
    } catch (const std::exception&) {
        throw FileIoException() << "Failed to save the project to: " << pathToProjectFile;
    }
}

std::string babelwires::ProjectSerialization::saveToString(const std::filesystem::path& pathToProjectFile, ProjectData projectData) {
    std::ostringstream os;
    internal::saveToStream(os, pathToProjectFile, std::move(projectData));
    return os.str();
}
