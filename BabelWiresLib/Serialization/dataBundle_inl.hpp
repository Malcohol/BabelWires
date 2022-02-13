#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

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

            void operator()(babelwires::Identifier& identifier) override { visit(identifier); }
            void operator()(babelwires::LongIdentifier& identifier) override { visit(identifier); }

            const SOURCE_REG& m_sourceReg;
            TARGET_REG& m_targetReg;
            babelwires::IdentifierRegistry::Authority m_authority;
        };

        template <typename DATA, typename SOURCE_REG, typename TARGET_REG>
        void convertData(DATA& data, SOURCE_REG&& sourceReg, TARGET_REG&& targetReg,
                         babelwires::IdentifierRegistry::Authority authority) {
            IdentifierVisitor<SOURCE_REG, TARGET_REG> visitor(sourceReg, targetReg, authority);
            data.visitIdentifiers(visitor);
        }
    } // namespace Detail
} // namespace babelwires

template <typename DATA>
babelwires::DataBundle<DATA>::DataBundle(std::filesystem::path pathToProjectFile, DATA&& data)
    : m_data(std::move(data))
    , m_projectFilePath(pathToProjectFile) {
    interpretIdentifiersInCurrentContext();
    interpretFilePathsInCurrentProjectPath();
}

template <typename DATA>
DATA babelwires::DataBundle<DATA>::resolveAgainstCurrentContext(const ProjectContext& context,
                                                                      const std::filesystem::path& pathToProjectFile,
                                                                      UserLogger& userLogger) && {
    resolveIdentifiersAgainstCurrentContext();
    resolveFilePathsAgainstCurrentProjectPath(pathToProjectFile, userLogger);
    adaptDataToAdditionalMetadata(context, userLogger);
    return std::move(m_data);
}

template <typename DATA> void babelwires::DataBundle<DATA>::interpretIdentifiersInCurrentContext() {
    IdentifierRegistry::ReadAccess sourceRegistry = IdentifierRegistry::read();
    try {
        Detail::convertData(m_data, sourceRegistry, &m_identifierRegistry, IdentifierRegistry::Authority::isTemporary);
    } catch (const ParseException&) {
        assert(false && "A field with a discriminator did not resolve.");
    }
}

template <typename DATA> void babelwires::DataBundle<DATA>::interpretFilePathsInCurrentProjectPath() {
    if (!m_projectFilePath.empty()) {
        const std::filesystem::path projectPath = m_projectFilePath;
        babelwires::FilePathVisitor visitor = [&](FilePath& filePath) {
            filePath.interpretRelativeTo(projectPath.parent_path());
        };
        m_data.visitFilePaths(visitor);
    }
}

template <typename DATA> void babelwires::DataBundle<DATA>::resolveIdentifiersAgainstCurrentContext() {
    IdentifierRegistry::WriteAccess targetRegistry = IdentifierRegistry::write();
    Detail::convertData(m_data, &m_identifierRegistry, targetRegistry, IdentifierRegistry::Authority::isProvisional);
}

template <typename DATA>
void babelwires::DataBundle<DATA>::resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToProjectFile,
                                                                       UserLogger& userLogger) {
    const std::filesystem::path newBase =
        pathToProjectFile.empty() ? std::filesystem::path() : pathToProjectFile.parent_path();
    const std::filesystem::path oldBase =
        m_projectFilePath.empty() ? std::filesystem::path() : std::filesystem::path(m_projectFilePath).parent_path();
    babelwires::FilePathVisitor visitor = [&](FilePath& filePath) {
        filePath.resolveRelativeTo(newBase, oldBase, userLogger);
    };
    m_data.visitFilePaths(visitor);
}

template <typename DATA> void babelwires::DataBundle<DATA>::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("filePath", m_projectFilePath);
    serializer.serializeObject(m_data);
    serializeAdditionalMetadata(serializer);
    serializer.serializeObject(m_identifierRegistry);
}

template <typename DATA> void babelwires::DataBundle<DATA>::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("filePath", m_projectFilePath, babelwires::Deserializer::IsOptional::Optional);
    m_data = std::move(*deserializer.deserializeObject<DATA>(DATA::serializationType));
    deserializeAdditionalMetadata(deserializer);
    m_identifierRegistry =
        std::move(*deserializer.deserializeObject<IdentifierRegistry>(IdentifierRegistry::serializationType));
}
