/**
 * Functions for saving and loading data.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

template <typename BUNDLE>
typename babelwires::DataSerialization<BUNDLE>::Data babelwires::DataSerialization<BUNDLE>::loadFromStream(std::istream& is,
                                                                         const DataContext& context,
                                                                         const std::filesystem::path& pathToFile,
                                                                         UserLogger& userLogger) {
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    XmlDeserializer deserializer(str, context.m_deserializationReg, userLogger);
    try {
        auto projectBundle = deserializer.deserializeObject<BUNDLE>(BUNDLE::serializationType);
        assert(projectBundle);
        deserializer.finalize();
        return std::move(*projectBundle).resolveAgainstCurrentContext(context, pathToFile, userLogger);
    } catch (ParseException& e) {
        deserializer.addContextDescription(e);
        throw;
    }
}

template <typename BUNDLE>
typename babelwires::DataSerialization<BUNDLE>::Data babelwires::DataSerialization<BUNDLE>::loadFromFile(const std::filesystem::path& pathToFile,
                                                                       const DataContext& context,
                                                                       UserLogger& userLogger) {
    std::ifstream is(pathToFile);
    try {
        return loadFromStream(is, context, pathToFile, userLogger);
    } catch (ParseException& e) {
        e << " when reading the file \"" << pathToFile << "\"";
        throw;
    }
}

template <typename BUNDLE>
typename babelwires::DataSerialization<BUNDLE>::Data babelwires::DataSerialization<BUNDLE>::loadFromString(const std::string& string,
                                                                         const DataContext& context,
                                                                         const std::filesystem::path& pathToFile,
                                                                         UserLogger& userLogger) {
    std::istringstream is(string);
    return loadFromStream(is, context, pathToFile, userLogger);
}

template <typename BUNDLE>
void babelwires::DataSerialization<BUNDLE>::saveToStream(std::ostream& os, const std::filesystem::path& pathToFile, Data data) {
    XmlSerializer serializer;
    BUNDLE bundle(pathToFile, std::move(data));
    bundle.interpretInCurrentContext();
    serializer.serializeObject(bundle);
    serializer.write(os);
}

template <typename BUNDLE>
void babelwires::DataSerialization<BUNDLE>::saveToFile(const std::filesystem::path& pathToFile, Data data) {
    try {
        OutFileStream os(pathToFile);
        saveToStream(os, pathToFile, std::move(data));
        os.close();
    } catch (const std::exception&) {
        throw FileIoException() << "Failed to save the project to: " << pathToFile;
    }
}

template <typename BUNDLE>
std::string babelwires::DataSerialization<BUNDLE>::saveToString(const std::filesystem::path& pathToFile, Data data) {
    std::ostringstream os;
    saveToStream(os, pathToFile, std::move(data));
    return os.str();
}
