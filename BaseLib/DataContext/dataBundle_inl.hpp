#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

#include <vector>

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
                    if (const auto fieldDataResult = m_sourceReg->getDeserializedIdentifierData(sourceId)) {
                        const auto& fieldData = *fieldDataResult;
                        newId = m_targetReg->addIdentifierWithMetadata(newId, *std::get<1>(fieldData),
                                                                       *std::get<2>(fieldData), m_authority);
                        // TODO Shouldn't this be sourceId = IDENTIFIER::tryMakeFrom(newId)?
                        sourceId.setDiscriminator(newId.getDiscriminator());
                    } else {
                        // In the saving case (isTemporary) an unregistered identifier means something is broken.
                        assert((m_authority != babelwires::IdentifierRegistry::Authority::isTemporary) &&
                               "A field with a discriminator did not resolve.");
                        // In the loading case (isProvisional), log the identifier so we can issue an error later.
                        m_unresolvedIdentifiers.emplace_back(sourceId);
                    }
                }
            }

            void operator()(babelwires::ShortId& identifier) override { visit(identifier); }
            void operator()(babelwires::MediumId& identifier) override { visit(identifier); }
            void operator()(babelwires::LongId& identifier) override { visit(identifier); }

            Result getResult() const {
                if (!m_unresolvedIdentifiers.empty()) {
                    Error error; 
                    error << "The following identifiers with discriminators did not resolve: ";
                    for (const auto& id : m_unresolvedIdentifiers) {
                        error << id << " ";
                    }
                    return error;
                }
                return {};
            }

            const SOURCE_REG& m_sourceReg;
            TARGET_REG& m_targetReg;
            babelwires::IdentifierRegistry::Authority m_authority;
            std::vector<LongId> m_unresolvedIdentifiers;
        };
    } // namespace Detail
} // namespace babelwires

template <typename DATA>
babelwires::DataBundle<DATA>::DataBundle(std::filesystem::path pathToFile, DATA&& data)
    : m_data(std::move(data))
    , m_pathToFile(pathToFile) {}

template <typename DATA> void babelwires::DataBundle<DATA>::interpretInCurrentContext() {
    interpretIdentifiersInCurrentContext();
    interpretFilePathsInCurrentProjectPath();
    interpretAdditionalMetadataInCurrentContext();
}

template <typename DATA>
babelwires::ResultT<DATA> babelwires::DataBundle<DATA>::resolveAgainstCurrentContext(const DataContext& context,
                                                                const std::filesystem::path& pathToFile,
                                                                UserLogger& userLogger) && {
    DO_OR_ERROR(resolveIdentifiersAgainstCurrentContext());
    resolveFilePathsAgainstCurrentProjectPath(pathToFile, userLogger);
    adaptDataToAdditionalMetadata(context, userLogger);
    return std::move(m_data);
}

template <typename DATA>
template <typename SOURCE_REG, typename TARGET_REG>
babelwires::Result babelwires::DataBundle<DATA>::convertIdentifiers(SOURCE_REG&& sourceReg, TARGET_REG&& targetReg,
                                                      babelwires::IdentifierRegistry::Authority authority) {
    Detail::IdentifierVisitor<SOURCE_REG, TARGET_REG> visitor(sourceReg, targetReg, authority);
    visitIdentifiers(visitor);
    return visitor.getResult();
}

template <typename DATA> void babelwires::DataBundle<DATA>::interpretIdentifiersInCurrentContext() {
    IdentifierRegistry::ReadAccess sourceRegistry = IdentifierRegistry::read();
#ifndef NDEBUG
    auto result = 
#endif
    convertIdentifiers(sourceRegistry, &m_identifierRegistry, IdentifierRegistry::Authority::isTemporary);
    // This problem should be caught by an earlier assert, but we check it here just in case.
    assert(result && "The current context's IdentifierRegistry is missing identifiers that are present in the data");
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

template <typename DATA> babelwires::Result babelwires::DataBundle<DATA>::resolveIdentifiersAgainstCurrentContext() {
    IdentifierRegistry::WriteAccess targetRegistry = IdentifierRegistry::write();
    return convertIdentifiers(&m_identifierRegistry, targetRegistry, IdentifierRegistry::Authority::isProvisional);
}

template <typename DATA>
void babelwires::DataBundle<DATA>::resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToFile,
                                                                             UserLogger& userLogger) {
    const std::filesystem::path newBase = pathToFile.empty() ? std::filesystem::path() : pathToFile.parent_path();
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

template <typename DATA>
babelwires::Result babelwires::DataBundle<DATA>::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.tryDeserializeValue("filePath", m_pathToFile));
    DO_OR_ERROR(deserializer.deserializeObjectByValue<DATA>(m_data, DATA::serializationType));
    DO_OR_ERROR(deserializeAdditionalMetadata(deserializer));
    DO_OR_ERROR(deserializer.deserializeObjectByValue<IdentifierRegistry>(m_identifierRegistry,
                                                                          IdentifierRegistry::serializationType));
    return {};
}
