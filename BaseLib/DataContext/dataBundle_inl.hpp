#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

namespace babelwires {
    namespace Detail {

        template <typename SOURCE_REG, typename TARGET_REG> struct IdentifierVisitor : babelwires::IdentifierVisitor {
            IdentifierVisitor(const SOURCE_REG& sourceReg, TARGET_REG& targetReg,
                              babelwires::IdentifierRegistry::Authority authority)
                : m_sourceReg(sourceReg)
                , m_targetReg(targetReg)
                , m_authority(authority) {}

            template <typename IDENTIFIER> void visit(IDENTIFIER& sourceId) {
                if (sourceId.getDiscriminator() != 0) {
                    IDENTIFIER newId = sourceId;
                    newId.setDiscriminator(0);
                    // This can throw, but an exception here is only meaningful in the loading case.
                    // In the saving case, the exception is caught and triggers an assertion.
                    const babelwires::IdentifierRegistry::ValueType fieldData =
                        m_sourceReg->getDeserializedIdentifierData(sourceId);
                    newId = m_targetReg->addIdentifierWithMetadata(newId, *std::get<1>(fieldData),
                                                                   *std::get<2>(fieldData), m_authority);
                    sourceId.setDiscriminator(newId.getDiscriminator());
                }
            }

            void operator()(babelwires::ShortId& identifier) override { visit(identifier); }
            void operator()(babelwires::MediumId& identifier) override { visit(identifier); }
            void operator()(babelwires::LongId& identifier) override { visit(identifier); }

            const SOURCE_REG& m_sourceReg;
            TARGET_REG& m_targetReg;
            babelwires::IdentifierRegistry::Authority m_authority;
        };
    } // namespace Detail
} // namespace babelwires

template <typename DATA>
babelwires::DataBundle<DATA>::DataBundle(std::filesystem::path pathToFile, DATA&& data)
    : m_data(std::move(data))
    , m_pathToFile(pathToFile) {
}

template <typename DATA>
void babelwires::DataBundle<DATA>::interpretInCurrentContext() {
    interpretIdentifiersInCurrentContext();
    interpretFilePathsInCurrentProjectPath();
    interpretAdditionalMetadataInCurrentContext();
}

template <typename DATA>
DATA babelwires::DataBundle<DATA>::resolveAgainstCurrentContext(const DataContext& context,
                                                                      const std::filesystem::path& pathToFile,
                                                                      UserLogger& userLogger) && {
    resolveIdentifiersAgainstCurrentContext();
    resolveFilePathsAgainstCurrentProjectPath(pathToFile, userLogger);
    adaptDataToAdditionalMetadata(context, userLogger);
    return std::move(m_data);
}

template <typename DATA>
template <typename SOURCE_REG, typename TARGET_REG>
void babelwires::DataBundle<DATA>::convertIdentifiers(SOURCE_REG&& sourceReg, TARGET_REG&& targetReg,
                    babelwires::IdentifierRegistry::Authority authority) {
    Detail::IdentifierVisitor<SOURCE_REG, TARGET_REG> visitor(sourceReg, targetReg, authority);
    visitIdentifiers(visitor);
}

template <typename DATA> void babelwires::DataBundle<DATA>::interpretIdentifiersInCurrentContext() {
    IdentifierRegistry::ReadAccess sourceRegistry = IdentifierRegistry::read();
    try {
        convertIdentifiers(sourceRegistry, &m_identifierRegistry, IdentifierRegistry::Authority::isTemporary);
    } catch (const ParseException&) {
        assert(false && "A field with a discriminator did not resolve.");
    }
}

template <typename DATA> void babelwires::DataBundle<DATA>::interpretFilePathsInCurrentProjectPath() {
    if (!m_pathToFile.empty()) {
        const std::filesystem::path projectPath = m_pathToFile;
        babelwires::FilePathVisitor visitor = [&](FilePath& filePath) {
            filePath.interpretRelativeTo(projectPath.parent_path());
        };
        visitFilePaths(visitor);
    }
}

template <typename DATA> void babelwires::DataBundle<DATA>::resolveIdentifiersAgainstCurrentContext() {
    IdentifierRegistry::WriteAccess targetRegistry = IdentifierRegistry::write();
    convertIdentifiers(&m_identifierRegistry, targetRegistry, IdentifierRegistry::Authority::isProvisional);
}

template <typename DATA>
void babelwires::DataBundle<DATA>::resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToFile,
                                                                       UserLogger& userLogger) {
    const std::filesystem::path newBase =
        pathToFile.empty() ? std::filesystem::path() : pathToFile.parent_path();
    const std::filesystem::path oldBase =
        m_pathToFile.empty() ? std::filesystem::path() : std::filesystem::path(m_pathToFile).parent_path();
    babelwires::FilePathVisitor visitor = [&](FilePath& filePath) {
        filePath.resolveRelativeTo(newBase, oldBase, userLogger);
    };
    visitFilePaths(visitor);
}

template <typename DATA> void babelwires::DataBundle<DATA>::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("filePath", m_pathToFile);
    serializer.serializeObject(m_data);
    serializeAdditionalMetadata(serializer);
    serializer.serializeObject(m_identifierRegistry);
}

template <typename DATA> babelwires::Result babelwires::DataBundle<DATA>::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.tryDeserializeValue("filePath", m_pathToFile));
    if (auto dataPtr = deserializer.deserializeObject<DATA>(DATA::serializationType)) {
        m_data = std::move(*dataPtr);
    }
    deserializeAdditionalMetadata(deserializer);
    if (auto identRegistryPtr = deserializer.deserializeObject<IdentifierRegistry>(IdentifierRegistry::serializationType)) {
        m_identifierRegistry = std::move(*identRegistryPtr);
    }
    return {};
}
