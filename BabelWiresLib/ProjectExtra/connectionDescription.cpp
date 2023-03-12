/**
 * ConnectionDescription is a self-contained description of a connection, useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeElementCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <Common/Hash/hash.hpp>

#include <cassert>

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::ConnectionDescription::getConnectionCommand() const {
    auto modifier = std::make_unique<babelwires::ConnectionModifierData>();
    modifier->m_sourceId = m_sourceId;
    modifier->m_pathToFeature = m_pathToTargetFeature;
    modifier->m_pathToSourceFeature = m_pathToSourceFeature;
    return std::make_unique<babelwires::AddModifierCommand>("Add connection", m_targetId, std::move(modifier));
}

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::ConnectionDescription::getDisconnectionCommand() const {
    return std::make_unique<RemoveElementCommand>("Remove connection", *this);
}

babelwires::ConnectionDescription::ConnectionDescription() = default;
babelwires::ConnectionDescription::ConnectionDescription(const ConnectionDescription& other) = default;
babelwires::ConnectionDescription::ConnectionDescription(ConnectionDescription&& other) = default;

babelwires::ConnectionDescription& babelwires::ConnectionDescription::operator=(const ConnectionDescription& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    m_pathToSourceFeature = other.m_pathToSourceFeature;
    m_pathToTargetFeature = other.m_pathToTargetFeature;
    return *this;
}

babelwires::ConnectionDescription& babelwires::ConnectionDescription::operator=(ConnectionDescription&& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    m_pathToSourceFeature = std::move(other.m_pathToSourceFeature);
    m_pathToTargetFeature = std::move(other.m_pathToTargetFeature);
    return *this;
}

babelwires::ConnectionDescription::ConnectionDescription(ElementId targetId, const ConnectionModifierData& data)
    : m_sourceId(data.m_sourceId)
    , m_targetId(targetId)
    , m_pathToSourceFeature(data.m_pathToSourceFeature)
    , m_pathToTargetFeature(data.m_pathToFeature) {}

bool babelwires::ConnectionDescription::operator==(const ConnectionDescription& other) const {
    return (m_sourceId == other.m_sourceId) && (m_targetId == other.m_targetId) &&
           (m_pathToSourceFeature == other.m_pathToSourceFeature) &&
           (m_pathToTargetFeature == other.m_pathToTargetFeature);
}

bool babelwires::ConnectionDescription::operator!=(const ConnectionDescription& other) const {
    return !(*this == other);
}

std::size_t babelwires::ConnectionDescription::getHash() const {
    std::size_t seed = 0xb238ea78;
    hash::mixInto(seed, m_sourceId, m_targetId, m_pathToSourceFeature, m_pathToTargetFeature);
    return seed;
}
