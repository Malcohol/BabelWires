/**
 * The command which removes content from a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeElementCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/node.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>


#include <algorithm>
#include <cassert>

babelwires::RemoveElementCommand::RemoveElementCommand(std::string commandName)
    : SimpleCommand(commandName) {}

babelwires::RemoveElementCommand::RemoveElementCommand(std::string commandName, ElementId elementId)
    : SimpleCommand(commandName)
    , m_elementIds{elementId} {}

babelwires::RemoveElementCommand::RemoveElementCommand(std::string commandName, ConnectionDescription connection)
    : SimpleCommand(commandName)
    , m_connections{std::move(connection)} {}

// Needed because ElementData is not declared in the header.
babelwires::RemoveElementCommand::~RemoveElementCommand() = default;

bool babelwires::RemoveElementCommand::addConnection(const babelwires::ConnectionDescription& desc,
                                                     ConnectionSet& connectionSet, const babelwires::Project& project) {
    const Node* targetElement = project.getFeatureElement(desc.m_targetId);
    if (!targetElement) {
        return false;
    }

    const Node* sourceElement = project.getFeatureElement(desc.m_sourceId);
    if (!sourceElement) {
        return false;
    }

    const EditTree& targetEdits = targetElement->getEdits();
    const EditTree& sourceEdits = sourceElement->getEdits();

    if (!targetEdits.isExpanded(desc.m_targetPath)) {
        // The target is in a collapsed compound.

        // Is the source is in a collapsed compound? Actual model connections will have paths which
        // have the desc's source path as a prefix.
        const bool testSourceUsingPrefix = !sourceEdits.isExpanded(desc.m_sourcePath);

        const auto subtree = targetEdits.modifierRange<ConnectionModifier>(desc.m_targetPath);
        for (const auto* m : subtree) {
            const ConnectionModifierData& data = m->getModifierData();
            if (data.m_sourceId == desc.m_sourceId) {
                if ((testSourceUsingPrefix && (desc.m_sourcePath.isPrefixOf(data.m_sourcePath))) ||
                    (!testSourceUsingPrefix && (desc.m_sourcePath == data.m_sourcePath))) {
                    ConnectionDescription subtreeConnection(desc.m_targetId, m->getModifierData());
                    if (connectionSet.insert(subtreeConnection).second) {
                        m_connections.emplace_back(std::move(subtreeConnection));
                    }
                }
            }
        }
    } else {
        // In this case, there is a single connection, but the desc may not accurately
        // describe it if the source is collapsed. This is important when it is restored
        // on undo. Hence, we look up the actual connection and store it.
        const Modifier* const modifierAtTarget = targetEdits.findModifier(desc.m_targetPath);
        const ConnectionModifier* actualConnection = modifierAtTarget->asConnectionModifier();
        if (!actualConnection) {
            return false;
        }

        const ConnectionModifierData& data = actualConnection->getModifierData();
        if ((data.m_sourceId != desc.m_sourceId) ||
            !desc.m_sourcePath.isPrefixOf(data.m_sourcePath)) {
            return false;
        }

        ConnectionDescription actualConnectionDesc(desc.m_targetId, data);

        if (connectionSet.insert(actualConnectionDesc).second) {
            m_connections.emplace_back(std::move(actualConnectionDesc));
        }
    }
    return true;
}

bool babelwires::RemoveElementCommand::initialize(const Project& project) {
    assert(!(m_elementIds.empty() && m_connections.empty()) && "This command has nothing to remove.");
    ConnectionSet connectionsBeingRemoved;

    // We need to reevaluate the connections originally added, in case they refer to truncated
    // connections due to collapsed compound features in the UI.
    {
        std::vector<ConnectionDescription> currentConnections;
        currentConnections.swap(m_connections);

        for (auto& connection : currentConnections) {
            if (!addConnection(std::move(connection), connectionsBeingRemoved, project)) {
                // Something must have changed.
                return false;
            }
        }
    }

    for (auto elementId : m_elementIds) {
        const Node* element = project.getFeatureElement(elementId);

        if (!element) {
            return false;
        }

        std::unique_ptr<ElementData> newElementData = element->extractElementData();

        // Move any connections described in the connections into the m_connections vector.
        auto newEnd = std::remove_if(
            newElementData->m_modifiers.begin(), newElementData->m_modifiers.end(),
            [this, elementId, &connectionsBeingRemoved](const std::unique_ptr<ModifierData>& modData) {
                if (const auto* assignFromData = modData.get()->as<ConnectionModifierData>()) {
                    ConnectionDescription connection(elementId, *assignFromData);
                    if (connectionsBeingRemoved.insert(connection).second) {
                        m_connections.emplace_back(connection);
                    }
                    return true;
                }
                return false;
            });
        newElementData->m_modifiers.erase(newEnd, newElementData->m_modifiers.end());

        m_elementsToRestore.emplace_back(std::move(newElementData));
    }

    const Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
    for (auto elementId : m_elementIds) {
        const Node* element = project.getFeatureElement(elementId);
        auto it = connectionInfo.m_requiredFor.find(element);
        if (it != connectionInfo.m_requiredFor.end()) {
            for (const auto& connection : it->second) {
                const Node* const target = std::get<1>(connection);
                const ConnectionModifier* const cmod = std::get<0>(connection);
                const ConnectionModifierData& data = cmod->getModifierData();
                ConnectionDescription connectionDesc(target->getElementId(), data);
                if (connectionsBeingRemoved.insert(connectionDesc).second) {
                    m_connections.emplace_back(connectionDesc);
                }
            }
        }
    }

    return true;
}

void babelwires::RemoveElementCommand::execute(Project& project) const {
    for (const auto& connection : reverseIterate(m_connections)) {
        project.removeModifier(connection.m_targetId, connection.m_targetPath);
    }
    for (auto elementId : m_elementIds) {
        project.removeElement(elementId);
    }
}

void babelwires::RemoveElementCommand::undo(Project& project) const {
    for (const auto& elementData : m_elementsToRestore) {
        project.addFeatureElement(*elementData);
    }
    for (const auto& connection : m_connections) {
        ConnectionModifierData newModifier;
        newModifier.m_targetPath = connection.m_targetPath;
        newModifier.m_sourceId = connection.m_sourceId;
        newModifier.m_sourcePath = connection.m_sourcePath;
        project.addModifier(connection.m_targetId, newModifier);
    }
}

bool babelwires::RemoveElementCommand::shouldSubsume(const Command& subsequentCommand,
                                                     bool thisIsAlreadyExecuted) const {
    return !thisIsAlreadyExecuted && subsequentCommand.as<RemoveElementCommand>();
}

void babelwires::RemoveElementCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<RemoveElementCommand>() && "subsume should not have been called");
    RemoveElementCommand* removeElementCommand = static_cast<RemoveElementCommand*>(subsequentCommand.get());
    m_elementIds.insert(m_elementIds.end(), removeElementCommand->m_elementIds.begin(),
                        removeElementCommand->m_elementIds.end());
}

void babelwires::RemoveElementCommand::addElementToRemove(ElementId elementId) {
    m_elementIds.emplace_back(elementId);
}
