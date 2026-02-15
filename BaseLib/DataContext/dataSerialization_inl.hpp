/**
 * Functions for saving and loading data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

template <typename BUNDLE>
babelwires::ResultT<typename babelwires::DataSerialization<BUNDLE>::Data>
babelwires::DataSerialization<BUNDLE>::loadFromStream(std::istream& is, const DataContext& context,
                                                      const std::filesystem::path& pathToFile, UserLogger& userLogger) {
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    XmlDeserializer deserializer(context.m_deserializationReg, userLogger);
    ON_ERROR(deserializer.finalize(ErrorState::Error));
    DO_OR_ERROR(deserializer.parse(str));
    auto projectBundleResult = deserializer.deserializeObject<BUNDLE>(BUNDLE::serializationType);
    if (!projectBundleResult) {
        deserializer.augmentResultWithContext(projectBundleResult);
        return projectBundleResult.error();
    }
    DO_OR_ERROR(deserializer.finalize());
    auto projectBundle = std::move(*projectBundleResult);
    return std::move(*projectBundle).resolveAgainstCurrentContext(context, pathToFile, userLogger);
}

template <typename BUNDLE>
babelwires::ResultT<typename babelwires::DataSerialization<BUNDLE>::Data>
babelwires::DataSerialization<BUNDLE>::loadFromFile(const std::filesystem::path& pathToFile, const DataContext& context,
                                                    UserLogger& userLogger) {
    std::ifstream is(pathToFile);
    auto result = loadFromStream(is, context, pathToFile, userLogger);
    if (!result) {
        return Error() << result.error() << " when reading the file \"" << pathToFile << "\"";
    }
    return result;
}

template <typename BUNDLE>
babelwires::ResultT<typename babelwires::DataSerialization<BUNDLE>::Data>
babelwires::DataSerialization<BUNDLE>::loadFromString(const std::string& string, const DataContext& context,
                                                      const std::filesystem::path& pathToFile, UserLogger& userLogger) {
    std::istringstream is(string);
    return loadFromStream(is, context, pathToFile, userLogger);
}

template <typename BUNDLE>
void babelwires::DataSerialization<BUNDLE>::saveToStream(std::ostream& os, const std::filesystem::path& pathToFile,
                                                         Data data) {
    XmlSerializer serializer;
    BUNDLE bundle(pathToFile, std::move(data));
    bundle.interpretInCurrentContext();
    serializer.serializeObject(bundle);
    serializer.write(os);
}

template <typename BUNDLE>
babelwires::Result babelwires::DataSerialization<BUNDLE>::saveToFile(const std::filesystem::path& pathToFile,
                                                                      Data data) {
    std::ofstream os(pathToFile);
    saveToStream(os, pathToFile, std::move(data));
    os.close();
    if (!os.good()) {
        return Error() << "Failed to save data to: " << pathToFile;
    }
    return {};
}

template <typename BUNDLE>
std::string babelwires::DataSerialization<BUNDLE>::saveToString(const std::filesystem::path& pathToFile, Data data) {
    std::ostringstream os;
    saveToStream(os, pathToFile, std::move(data));
    return os.str();
}
