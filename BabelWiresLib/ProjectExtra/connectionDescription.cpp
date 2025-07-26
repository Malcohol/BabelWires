/**
 * ConnectionDescription is a self-contained description of a connection, useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>

#include <BabelWiresLib/Project/Commands/addConnectionCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeNodeCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <Common/Hash/hash.hpp>

#include <cassert>

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::ConnectionDescription::getConnectionCommand() const {
    auto modifier = std::make_unique<babelwires::ConnectionModifierData>();
    modifier->m_sourceId = m_sourceId;
    modifier->m_targetPath = m_targetPath;
    modifier->m_sourcePath = m_sourcePath;
    return std::make_unique<babelwires::AddConnectionCommand>("Add connection", m_targetId, std::move(modifier));
}

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::ConnectionDescription::getDisconnectionCommand() const {
    return std::make_unique<RemoveNodeCommand>("Remove connection", *this);
}

babelwires::ConnectionDescription::ConnectionDescription() = default;
babelwires::ConnectionDescription::ConnectionDescription(const ConnectionDescription& other) = default;
babelwires::ConnectionDescription::ConnectionDescription(ConnectionDescription&& other) = default;

babelwires::ConnectionDescription::ConnectionDescription(NodeId sourceId, Path sourcePath, NodeId targetId, Path targetPath)
    : m_sourceId(sourceId), m_sourcePath(std::move(sourcePath)), m_targetId(targetId), m_targetPath(std::move(targetPath)) {}

babelwires::ConnectionDescription& babelwires::ConnectionDescription::operator=(const ConnectionDescription& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    m_sourcePath = other.m_sourcePath;
    m_targetPath = other.m_targetPath;
    return *this;
}

babelwires::ConnectionDescription& babelwires::ConnectionDescription::operator=(ConnectionDescription&& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    m_sourcePath = std::move(other.m_sourcePath);
    m_targetPath = std::move(other.m_targetPath);
    return *this;
}

babelwires::ConnectionDescription::ConnectionDescription(NodeId targetId, const ConnectionModifierData& data)
    : m_sourceId(data.m_sourceId)
    , m_targetId(targetId)
    , m_sourcePath(data.m_sourcePath)
    , m_targetPath(data.m_targetPath) {}

bool babelwires::ConnectionDescription::operator==(const ConnectionDescription& other) const {
    return (m_sourceId == other.m_sourceId) && (m_targetId == other.m_targetId) &&
           (m_sourcePath == other.m_sourcePath) &&
           (m_targetPath == other.m_targetPath);
}

bool babelwires::ConnectionDescription::operator!=(const ConnectionDescription& other) const {
    return !(*this == other);
}

std::size_t babelwires::ConnectionDescription::getHash() const {
    std::size_t seed = 0xb238ea78;
    hash::mixInto(seed, m_sourceId, m_targetId, m_sourcePath, m_targetPath);
    return seed;
}
