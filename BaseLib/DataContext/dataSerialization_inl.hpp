/**
 * Functions for saving and loading data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

template <typename BUNDLE>
babelwires::ResultT<typename babelwires::DataSerialization<BUNDLE>::Data>
babelwires::DataSerialization<BUNDLE>::loadFromStream(std::istream& is, const Context& context,
                                                      const std::filesystem::path& pathToFile, UserLogger& userLogger) {
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    const DeserializationRegistry& deserializationRegistry = context.get<DeserializationRegistry>();
    auto deserializer = UserDocumentSerializationFactory::createDeserializer(deserializationRegistry, userLogger);
    assert(deserializer && "UserDocumentSerializationFactory returned a null deserializer");

    ON_ERROR(deserializer->finalizeOnError());
    DO_OR_ERROR(deserializer->parse(str));
    auto projectBundleResult = deserializer->template deserializeObject<BUNDLE>(BUNDLE::s_serializationTypeName);
    if (!projectBundleResult) {
        deserializer->augmentResultWithContext(projectBundleResult);
        RETURN_ERROR_VALUE(projectBundleResult.error());
    }
    DO_OR_ERROR(deserializer->finalize());
    auto projectBundle = std::move(*projectBundleResult);
    return std::move(*projectBundle).resolveAgainstCurrentContext(context, pathToFile, userLogger);
}

template <typename BUNDLE>
babelwires::ResultT<typename babelwires::DataSerialization<BUNDLE>::Data>
babelwires::DataSerialization<BUNDLE>::loadFromFile(const std::filesystem::path& pathToFile, const Context& context,
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
babelwires::DataSerialization<BUNDLE>::loadFromString(const std::string& string, const Context& context,
                                                      const std::filesystem::path& pathToFile, UserLogger& userLogger) {
    std::istringstream is(string);
    return loadFromStream(is, context, pathToFile, userLogger);
}

template <typename BUNDLE>
void babelwires::DataSerialization<BUNDLE>::saveToStream(std::ostream& os, const Context& context,
                                                         const std::filesystem::path& pathToFile, Data data) {
    auto serializer = UserDocumentSerializationFactory::createSerializer();
    assert(serializer && "UserDocumentSerializationFactory returned a null serializer");

    BUNDLE bundle(pathToFile, std::move(data));
    bundle.interpretInCurrentContext();
    serializer->serializeObject(bundle);
    serializer->write(os);
}

template <typename BUNDLE>
babelwires::Result babelwires::DataSerialization<BUNDLE>::saveToFile(const std::filesystem::path& pathToFile,
                                                                     const Context& context, Data data) {
    std::ofstream os(pathToFile);
    saveToStream(os, context, pathToFile, std::move(data));
    os.close();
    if (!os.good()) {
        return Error() << "Failed to save data to: " << pathToFile;
    }
    return {};
}

template <typename BUNDLE>
std::string babelwires::DataSerialization<BUNDLE>::saveToString(const std::filesystem::path& pathToFile,
                                                                const Context& context, Data data) {
    std::ostringstream os;
    saveToStream(os, context, pathToFile, std::move(data));
    return os.str();
}
