/**
 * ConnectionDescription is a self-contained description of a connection, useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Commands/commands.hpp>

namespace babelwires {
    class Project;
    struct ConnectionModifierData;

    /// ConnectionDescription is a self-contained description of a connection.
    /// The project does not use this (instead it uses a ConnectionModifier stored at the target Node).
    struct ConnectionDescription {
        ConnectionDescription();
        ConnectionDescription(const ConnectionDescription& other);
        ConnectionDescription(ConnectionDescription&& other);
        ConnectionDescription(NodeId targetId, const ConnectionModifierData& data);
        ConnectionDescription& operator=(const ConnectionDescription& other);
        ConnectionDescription& operator=(ConnectionDescription&& other);

        /// Create a command which would add this connection.
        std::unique_ptr<babelwires::Command<Project>> getConnectionCommand() const;

        /// Create a command which would remove this connection.
        std::unique_ptr<babelwires::Command<Project>> getDisconnectionCommand() const;

        bool operator==(const ConnectionDescription& other) const;
        bool operator!=(const ConnectionDescription& other) const;
        std::size_t getHash() const;

        NodeId m_sourceId = INVALID_ELEMENT_ID;
        Path m_sourcePath;
        NodeId m_targetId = INVALID_ELEMENT_ID;
        Path m_targetPath;
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ConnectionDescription> {
        inline std::size_t operator()(const babelwires::ConnectionDescription& info) const { return info.getHash(); }
    };
} // namespace std
