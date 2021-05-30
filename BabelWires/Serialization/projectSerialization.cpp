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

babelwires::ProjectData babelwires::ProjectSerialization::loadFromStream(std::istream& is,
                                                                         const ProjectContext& context,
                                                                         UserLogger& userLogger) {
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    XmlDeserializer deserializer(str, context.m_deserializationReg, userLogger);
    try {
        auto projectBundle = deserializer.deserializeObject<ProjectBundle>(ProjectBundle::serializationType);
        assert(projectBundle);
        deserializer.finalize();
        return std::move(*projectBundle).resolveAgainstCurrentContext(context, userLogger);
    } catch (ParseException& e) {
        deserializer.addContextDescription(e);
        throw;
    }
}

babelwires::ProjectData babelwires::ProjectSerialization::loadFromFile(const std::filesystem::path& filePath,
                                                                       const ProjectContext& context,
                                                                       UserLogger& userLogger) {
    std::ifstream is(filePath);
    try {
        return loadFromStream(is, context, userLogger);
    } catch (ParseException& e) {
        e << " when reading the file \"" << filePath << "\"";
        throw;
    }
}

babelwires::ProjectData babelwires::ProjectSerialization::loadFromString(const std::string& string,
                                                                         const ProjectContext& context,
                                                                         UserLogger& userLogger) {
    std::istringstream is(string);
    return loadFromStream(is, context, userLogger);
}

void babelwires::ProjectSerialization::saveToStream(std::ostream& os, ProjectData projectData) {
    XmlSerializer serializer;
    ProjectBundle bundle(std::move(projectData));
    serializer.serializeObject(bundle);
    serializer.write(os);
}

void babelwires::ProjectSerialization::saveToFile(const std::filesystem::path& filePath, ProjectData projectData) {
    try {
        OutFileStream os(filePath);
        saveToStream(os, std::move(projectData));
        os.close();
    } catch (const std::exception& e) {
        throw FileIoException() << "Failed to save the project to: " << filePath;
    }
}

std::string babelwires::ProjectSerialization::saveToString(ProjectData projectData) {
    std::ostringstream os;
    saveToStream(os, std::move(projectData));
    return os.str();
}
