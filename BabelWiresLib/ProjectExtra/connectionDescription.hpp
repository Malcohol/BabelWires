/**
 * ConnectionDescription is a self-contained description of a connection, useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/Path/featurePath.hpp"
#include "BabelWiresLib/Project/projectIds.hpp"

namespace babelwires {

    struct ConnectionModifierData;
    class Command;

    /// ConnectionDescription is a self-contained description of a connection.
    /// The project does not use this (instead it uses a ConnectionModifier stored at the target FeatureElement).
    struct ConnectionDescription {
        ConnectionDescription();
        ConnectionDescription(const ConnectionDescription& other);
        ConnectionDescription(ConnectionDescription&& other);
        ConnectionDescription(ElementId targetId, const ConnectionModifierData& data);
        ConnectionDescription& operator=(const ConnectionDescription& other);
        ConnectionDescription& operator=(ConnectionDescription&& other);

        /// Create a command which would add this connection.
        std::unique_ptr<babelwires::Command> getConnectionCommand() const;

        /// Create a command which would remove this connection.
        std::unique_ptr<babelwires::Command> getDisconnectionCommand() const;

        bool operator==(const ConnectionDescription& other) const;
        bool operator!=(const ConnectionDescription& other) const;
        std::size_t getHash() const;

        ElementId m_sourceId = INVALID_ELEMENT_ID;
        FeaturePath m_pathToSourceFeature;
        ElementId m_targetId = INVALID_ELEMENT_ID;
        FeaturePath m_pathToTargetFeature;
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ConnectionDescription> {
        inline std::size_t operator()(const babelwires::ConnectionDescription& info) const { return info.getHash(); }
    };
} // namespace std
