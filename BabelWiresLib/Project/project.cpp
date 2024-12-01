/**
 * The Project manages the graph of Nodes, and propagates data from sources to targets.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/project.hpp>

#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <Common/IO/fileDataSource.hpp>
#include <Common/Log/userLogger.hpp>
#include <Common/exceptions.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>
#include <unordered_set>

babelwires::Project::Project(ProjectContext& context, UserLogger& userLogger)
    : m_context(context)
    , m_userLogger(userLogger) {
    randomizeProjectId();
}

void babelwires::Project::updateWithAvailableIds(std::vector<NodeId>& idsInOut) const {
    NodeId maxAssignedNodeId = m_maxAssignedNodeId;
    std::unordered_set<NodeId> nowAssigned;
    for (auto& id : idsInOut) {
        if ((id == INVALID_ELEMENT_ID) || (m_nodes.find(id) != m_nodes.end()) ||
            (nowAssigned.find(id) != nowAssigned.end())) {
            ++maxAssignedNodeId;
            id = maxAssignedNodeId;
        } else {
            maxAssignedNodeId = std::max(maxAssignedNodeId, id);
        }
        nowAssigned.insert(id);
    }
}

babelwires::NodeId babelwires::Project::reserveNodeId(NodeId hint) {
    if ((hint == INVALID_ELEMENT_ID) || (m_nodes.find(hint) != m_nodes.end())) {
        ++m_maxAssignedNodeId;
        return m_maxAssignedNodeId;
    } else {
        m_maxAssignedNodeId = std::max(m_maxAssignedNodeId, hint);
        return hint;
    }
}

babelwires::Node* babelwires::Project::addNodeWithoutCachingConnection(const NodeData& data) {
    const NodeId availableId = reserveNodeId(data.m_id);
    std::unique_ptr<Node> nodePtr = data.createNode(m_context, m_userLogger, availableId);
    Node* node = nodePtr.get();
    m_nodes.insert(std::make_pair(availableId, std::move(nodePtr)));
    return node;
}

void babelwires::Project::addNodeConnectionsToCache(Node* node) {
    for (const auto& connectionModifier : node->getConnectionModifiers()) {
        addConnectionToCache(node, connectionModifier);
    }
}

babelwires::NodeId babelwires::Project::addNode(const NodeData& data) {
    Node* node = addNodeWithoutCachingConnection(data);
    addNodeConnectionsToCache(node);
    return node->getNodeId();
}

void babelwires::Project::removeNode(NodeId nodeId) {
    auto mapIt = m_nodes.find(nodeId);
    assert((mapIt != m_nodes.end()) && "nodeId must refer to an node in the project");
    Node* node = mapIt->second.get();

    for (const auto& connectionModifier : node->getConnectionModifiers()) {
        removeConnectionFromCache(node, connectionModifier);
    }

    m_removedNodes.insert(std::move(*mapIt));
    m_nodes.erase(mapIt);
}

void babelwires::Project::addModifier(NodeId nodeId, const ModifierData& modifierData) {
    Node* node = getNode(nodeId);
    assert(node && "Modifier added to unregistered feature node");
    Modifier* modifier = node->addModifier(m_userLogger, modifierData);
    if (ConnectionModifier* connectionModifier = modifier->asConnectionModifier()) {
        addConnectionToCache(node, connectionModifier);
    }
}

void babelwires::Project::removeModifier(NodeId nodeId, const Path& featurePath) {
    Node* const node = getNode(nodeId);
    assert(node && "Cannot remove a modifier from an node that does not exist");
    Modifier* const modifier = node->findModifier(featurePath);
    assert(modifier && "Cannot remove a modifier that does not exist");
    if (ConnectionModifier* connectionModifier = modifier->asConnectionModifier()) {
        removeConnectionFromCache(node, connectionModifier);
    }
    node->removeModifier(modifier);
}

void babelwires::Project::adjustModifiersInArrayElements(NodeId nodeId, const babelwires::Path& pathToArray,
                                    babelwires::ArrayIndex startIndex, int adjustment) {
    if (adjustment < 0) {
        startIndex -= adjustment;
    }
    Node* node = getNode(nodeId);
    node->adjustArrayIndices(pathToArray, startIndex, adjustment);

    // Adjust modifiers of other elements affected by the addition.
    // The addition would seem to affect only input features, so this may seem unneccessary.
    // However, we allow input and output features of processors to share paths,
    // in which case, we want any matching output connections to update too.
    const auto r = m_connectionCache.m_requiredFor.find(node);
    if (r != m_connectionCache.m_requiredFor.end()) {
        const babelwires::Project::ConnectionInfo::Connections& connections = r->second;
        for (auto&& pair : connections) {
            babelwires::ConnectionModifier& modifier = *std::get<0>(pair);
            modifier.adjustSourceArrayIndices(pathToArray, startIndex, adjustment);
        }
    }
}

// TODO These assume that the operation should always apply to output features.
// If this assumption is not valid, we may need to make the stated assumptions about how input/output paths match
// tighter.

void babelwires::Project::addArrayEntries(NodeId nodeId, const Path& pathToArray, int indexOfNewElement,
                                          int numEntriesToAdd, bool ensureModifier) {
    assert((indexOfNewElement >= 0) && "indexOfNewElement must be positive");
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");

    if (Node* const node = getNode(nodeId)) {
        if (ValueTreeNode* const inputArray = node->getInputNonConst(pathToArray)) {
            ValueTreeNode* valueTreeNode = pathToArray.tryFollow(*inputArray);
            assert(valueTreeNode && "Path should resolve");
            assert(valueTreeNode->getType().as<ArrayType>());

            // First, ensure there is an appropriate modifier at the array.
            ArraySizeModifier* arrayModifier = nullptr;
            if (Modifier* const modifier = node->findModifier(pathToArray)) {
                arrayModifier = modifier->as<ArraySizeModifier>();
                if (!arrayModifier && ensureModifier) {
                    // Defensive: Wasn't an array modifier at the path, so let it be replaced.
                    // This won't be restored by an undo, but that shouldn't matter since it isn't
                    // useful.
                    node->removeModifier(modifier);
                }
            }
            if (!arrayModifier) {
                if (ensureModifier) {
                    ArraySizeModifierData arrayInitDataPtr;
                    arrayInitDataPtr.m_targetPath = pathToArray;
                    arrayInitDataPtr.m_size = valueTreeNode->getNumChildren();
                    arrayModifier =
                        static_cast<ArraySizeModifier*>(node->addModifier(m_userLogger, arrayInitDataPtr));
                }
            }

            // Next, set the array size.
            if (arrayModifier)
            { 
                arrayModifier->addArrayEntries(m_userLogger, inputArray, indexOfNewElement, numEntriesToAdd); 
            }

            if (arrayModifier && !ensureModifier) {
                node->removeModifier(arrayModifier);
            }
        }
    }
}

void babelwires::Project::removeArrayEntries(NodeId nodeId, const Path& pathToArray,
                                             int indexOfElementToRemove, int numEntriesToRemove, bool ensureModifier) {
    assert((indexOfElementToRemove >= 0) && "indexOfEntriesToRemove must be positive");
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    if (Node* const node = getNode(nodeId)) {
        if (ValueTreeNode* const inputArray = node->getInputNonConst(pathToArray)) {
            ValueTreeNode* valueTreeNode = pathToArray.tryFollow(*inputArray);
            assert(valueTreeNode && "Path should resolve");
            assert(valueTreeNode->getType().as<ArrayType>());

            // First, check if there is a modifier at the array.
            ArraySizeModifier* arrayModifier = nullptr;
            if (Modifier* const modifier = node->findModifier(pathToArray)) {
                arrayModifier = modifier->as<ArraySizeModifier>();
                if (!arrayModifier && ensureModifier) {
                    // Defensive: Wasn't an array modifier at the path, so let it be replaced.
                    // This won't be restored by an undo, but that shouldn't matter since it isn't
                    // useful.
                    node->removeModifier(modifier);
                }
            }
            if (!arrayModifier) {
                if (ensureModifier) {
                    ArraySizeModifierData arrayInitDataPtr;
                    arrayInitDataPtr.m_targetPath = pathToArray;
                    arrayInitDataPtr.m_size = valueTreeNode->getNumChildren();
                    arrayModifier =
                        static_cast<ArraySizeModifier*>(node->addModifier(m_userLogger, arrayInitDataPtr));
                }
            }

            // Next, set the array size.
            if (arrayModifier) {
                arrayModifier->removeArrayEntries(m_userLogger, inputArray, indexOfElementToRemove,
                                                  numEntriesToRemove);
            }

            if (arrayModifier && !ensureModifier) {
                node->removeModifier(arrayModifier);
            }
        }
    }
}

void babelwires::Project::setProjectData(const ProjectData& projectData) {
    clear();

    if (projectData.m_projectId != INVALID_PROJECT_ID) {
        m_projectId = projectData.m_projectId;
    }
    std::vector<Node*> nodesAdded;
    for (const auto& nodeData : projectData.m_nodes) {
        nodesAdded.emplace_back(addNodeWithoutCachingConnection(*nodeData));
    }
    for (auto* node : nodesAdded) {
        addNodeConnectionsToCache(node);
    }
}

babelwires::ProjectData babelwires::Project::extractProjectData() const {
    ProjectData projectData;
    projectData.m_projectId = m_projectId;
    for (const auto& pair : m_nodes) {
        projectData.m_nodes.emplace_back(pair.second->extractElementData());
    }
    return projectData;
}

void babelwires::Project::clear() {
    // TODO Why not just clear? This isn't undoable.
    m_removedNodes.swap(m_nodes);
    m_nodes.clear();
    setConnectionCacheInvalid();
    randomizeProjectId();
    m_maxAssignedNodeId = 0;
}

babelwires::Project::~Project() {}

const babelwires::TargetFileFormatRegistry& babelwires::Project::getFactoryFormatRegistry() const {
    return m_context.m_targetFileFormatReg;
}

const babelwires::SourceFileFormatRegistry& babelwires::Project::getFileFormatRegistry() const {
    return m_context.m_sourceFileFormatReg;
}

babelwires::Node* babelwires::Project::getNode(NodeId id) {
    auto&& it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

const babelwires::Node* babelwires::Project::getNode(NodeId id) const {
    auto&& it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

void babelwires::Project::tryToReloadAllSources() {
    int attemptedReloads = 0;
    int successfulReloads = 0;
    for (const auto& [_, f] : m_nodes) {
        if (FileNode* const fileNode = f->as<FileNode>()) {
            if (isNonzero(fileNode->getSupportedFileOperations() & FileNode::FileOperations::reload)) {
                ++attemptedReloads;
                if (fileNode->reload(m_context, m_userLogger)) {
                    ++successfulReloads;
                }
            }
        }
    }
    m_userLogger.logInfo() << "Reloaded " << successfulReloads << "/" << attemptedReloads << " files.";
}

void babelwires::Project::tryToSaveAllTargets() {
    int attemptedSaves = 0;
    int successfulSaves = 0;
    for (const auto& [_, f] : m_nodes) {
        if (FileNode* const fileNode = f->as<FileNode>()) {
            if (isNonzero(fileNode->getSupportedFileOperations() & FileNode::FileOperations::save)) {
                ++attemptedSaves;
                if (fileNode->save(m_context, m_userLogger)) {
                    ++successfulSaves;
                }
            }
        }
    }
    m_userLogger.logInfo() << "Saved " << successfulSaves << "/" << attemptedSaves << " files.";
}

void babelwires::Project::tryToReloadSource(NodeId id) {
    assert((id != INVALID_ELEMENT_ID) && "Invalid id");
    Node* f = getNode(id);
    assert(f && "There was no such feature node");
    FileNode* const fileNode = f->as<FileNode>();
    assert(fileNode && "There was no such file node");
    assert(isNonzero(fileNode->getSupportedFileOperations() & FileNode::FileOperations::reload) &&
           "There was no such reloadable file node");
    fileNode->reload(m_context, m_userLogger);
}

void babelwires::Project::tryToSaveTarget(NodeId id) {
    assert((id != INVALID_ELEMENT_ID) && "Invalid id");
    Node* f = getNode(id);
    assert(f && "There was no such feature node");
    FileNode* const fileNode = f->as<FileNode>();
    assert(fileNode && "There was no such file node");
    assert(isNonzero(fileNode->getSupportedFileOperations() & FileNode::FileOperations::save) &&
           "There was no such saveable file node");
    fileNode->save(m_context, m_userLogger);
}

void babelwires::Project::setConnectionCacheInvalid() {
    m_connectionCache.m_dependsOn.clear();
    m_connectionCache.m_requiredFor.clear();
    m_connectionCache.m_brokenConnections.clear();
}

void babelwires::Project::addConnectionToCache(Node* node, ConnectionModifier* connectionModifier) {
    if (auto source = getNode(connectionModifier->getModifierData().m_sourceId)) {
        {
            auto itAndBool = m_connectionCache.m_dependsOn.insert(
                std::make_pair(node, babelwires::Project::ConnectionInfo::Connections()));
            babelwires::Project::ConnectionInfo::Connections& connections = itAndBool.first->second;
            connections.emplace_back(std::make_tuple(connectionModifier, source));
        }
        {
            auto itAndBool = m_connectionCache.m_requiredFor.insert(
                std::make_pair(source, babelwires::Project::ConnectionInfo::Connections()));
            babelwires::Project::ConnectionInfo::Connections& connections = itAndBool.first->second;
            connections.emplace_back(std::make_tuple(connectionModifier, node));
        }
    } else {
        m_connectionCache.m_brokenConnections.emplace_back(std::make_tuple(connectionModifier, node));
    }
}

void babelwires::Project::removeConnectionFromCache(Node* node, ConnectionModifier* connectionModifier) {
    if (auto source = getNode(connectionModifier->getModifierData().m_sourceId)) {
        {
            auto dit = m_connectionCache.m_dependsOn.find(node);
            assert((dit != m_connectionCache.m_dependsOn.end()) && "Cannot find dependency with connection to remove");
            babelwires::Project::ConnectionInfo::Connections& connections = dit->second;
            auto cit = std::find(connections.begin(), connections.end(), std::make_tuple(connectionModifier, source));
            assert((cit != connections.end()) && "Cannot find connection to remove");
            connections.erase(cit);
            if (connections.empty()) {
                m_connectionCache.m_dependsOn.erase(dit);
            }
        }
        {
            auto rit = m_connectionCache.m_requiredFor.find(source);
            assert((rit != m_connectionCache.m_requiredFor.end()) &&
                   "Cannot find dependency with connection to remove");
            auto& connections = rit->second;
            auto cit = std::find(connections.begin(), connections.end(), std::make_tuple(connectionModifier, node));
            assert((cit != connections.end()) && "Cannot find connection to remove");
            connections.erase(cit);
            if (connections.empty()) {
                m_connectionCache.m_requiredFor.erase(rit);
            }
        }
    } else {
        auto it = std::find(m_connectionCache.m_brokenConnections.begin(), m_connectionCache.m_brokenConnections.end(),
                            std::make_tuple(connectionModifier, node));
        m_connectionCache.m_brokenConnections.erase(it);
    }
}

void babelwires::Project::validateConnectionCache() const {
#ifndef NDEBUG
    const auto checkOwned = [this](const Node* pointerToCheck) {
        const auto it =
            std::find_if(m_nodes.begin(), m_nodes.end(),
                         [pointerToCheck](const auto& uPtr) { return std::get<1>(uPtr).get() == pointerToCheck; });
        assert((it != m_nodes.end()) && "The cache refers to an node that is not owned by the project");
    };

    for (auto mapPair : m_connectionCache.m_dependsOn) {
        checkOwned(mapPair.first);
        for (auto connectionPair : mapPair.second) {
            checkOwned(std::get<1>(connectionPair));
        }
    }

    for (auto mapPair : m_connectionCache.m_requiredFor) {
        checkOwned(mapPair.first);
        for (auto connectionPair : mapPair.second) {
            checkOwned(std::get<1>(connectionPair));
        }
    }

    for (const auto& pair : m_nodes) {
        const Node* target = pair.second.get();

        for (const auto& connectionModifier : target->getConnectionModifiers()) {
            if (auto source = getNode(connectionModifier->getModifierData().m_sourceId)) {
                {
                    const auto dit = m_connectionCache.m_dependsOn.find(target);
                    assert((dit != m_connectionCache.m_dependsOn.end()) &&
                           "The connection cache is missing a dependsOn dependency");
                    const babelwires::Project::ConnectionInfo::Connections& connections = dit->second;
                    auto cit =
                        std::find(connections.begin(), connections.end(), std::make_tuple(connectionModifier, source));
                    assert((cit != connections.end()) && "The connection cache is missing a dependsOn connection");
                }
                {
                    const auto rit = m_connectionCache.m_requiredFor.find(source);
                    assert((rit != m_connectionCache.m_requiredFor.end()) &&
                           "The connection cache is missing a requiredFor dependency");
                    const auto& connections = rit->second;
                    const auto cit =
                        std::find(connections.begin(), connections.end(), std::make_tuple(connectionModifier, target));
                    assert((cit != connections.end()) && "The connection cache is missing a requiredFor connection");
                }
            } else {
                const auto it =
                    std::find(m_connectionCache.m_brokenConnections.begin(),
                              m_connectionCache.m_brokenConnections.end(), std::make_tuple(connectionModifier, target));
                assert((it != m_connectionCache.m_brokenConnections.end()) &&
                       "The connection cache is missing a broken connection");
            }
        }
    }
#endif // NDEBUG
}

const babelwires::Project::ConnectionInfo& babelwires::Project::getConnectionInfo() const {
    return m_connectionCache;
}

void babelwires::Project::propagateChanges(const Node* e) {
    const auto r = m_connectionCache.m_requiredFor.find(e);
    if (r != m_connectionCache.m_requiredFor.end()) {
        const ConnectionInfo::Connections& connections = r->second;
        for (auto&& pair : connections) {
            ConnectionModifier* connection = std::get<0>(pair);
            Node* targetElement = std::get<1>(pair);
            if (ValueTreeNode* input = targetElement->getInputNonConst(connection->getTargetPath())) {
                connection->applyConnection(*this, m_userLogger, input);
            }
        }
    }

    // Check that all broken connections are marked failed.
    for (auto pair : m_connectionCache.m_brokenConnections) {
        ConnectionModifier* connection = std::get<0>(pair);
        Node* owner = std::get<1>(pair);
        if (!connection->isFailed()) {
            if (ValueTreeNode* input = owner->getInputNonConst(connection->getTargetPath())) {
                connection->applyConnection(*this, m_userLogger, input);
            }
        }
    }
}

void babelwires::Project::process() {
    validateConnectionCache();

    // Topologically sort the featureElements in a later-depends-on-earlier order.
    std::vector<Node*> sortedElements;

    sortedElements.reserve(m_nodes.size());
    for (auto&& pair : m_nodes) {
        sortedElements.emplace_back(pair.second.get());
    }

    std::unordered_map<const Node*, int> numDependencies;
    numDependencies.reserve(m_connectionCache.m_dependsOn.size());
    for (auto&& pair : m_connectionCache.m_dependsOn) {
        numDependencies.insert(std::make_pair(pair.first, pair.second.size()));
    }

    // Because we're marking the elements as they are sorted, we have to iterate to the end.
    for (int firstUnsortedIndex = 0; firstUnsortedIndex < m_nodes.size();) {
        const int firstUnsortedIndexBefore = firstUnsortedIndex;
        for (int j = firstUnsortedIndex; j < m_nodes.size(); ++j) {
            Node* node = sortedElements[j];

            const auto it = numDependencies.find(node);
            if (it == numDependencies.end() || it->second == 0) {
                std::swap(sortedElements[firstUnsortedIndex], sortedElements[j]);
                node->setInDependencyLoop(false);
                ++firstUnsortedIndex;
                const auto r = m_connectionCache.m_requiredFor.find(node);
                if (r != m_connectionCache.m_requiredFor.end()) {
                    const ConnectionInfo::Connections& connections = r->second;
                    for (auto&& pair : connections) {
                        const auto sit = numDependencies.find(std::get<1>(pair));
                        if (sit != numDependencies.end()) {
                            --sit->second;
                        }
                    }
                }
            }
        }
        // Check for a dependency loop.
        if (firstUnsortedIndex == firstUnsortedIndexBefore) {
            for (int i = firstUnsortedIndex; i < m_nodes.size(); ++i) {
                sortedElements[i]->setInDependencyLoop(true);
            }
            break;
        }
    }

    // Now iterate in dependency order.
    for (auto&& node : sortedElements) {
        node->process(*this, m_userLogger);
        // Existing connections only apply their contents if their source has changed,
        // so this doesn't unnecessarily change dependent data.
        // We do need to visit all out-going connections in case some are new.
        propagateChanges(node);
    }
}

const std::map<babelwires::NodeId, std::unique_ptr<babelwires::Node>>&
babelwires::Project::getNodes() const {
    return m_nodes;
}

void babelwires::Project::clearChanges() {
    for (auto&& pair : m_nodes) {
        pair.second->clearChanges();
    }
    m_removedNodes.clear();
}

const std::map<babelwires::NodeId, std::unique_ptr<babelwires::Node>>&
babelwires::Project::getRemovedNodes() const {
    return m_removedNodes;
}

void babelwires::Project::setNodePosition(NodeId nodeId, const UiPosition& newPosition) {
    Node* featureElement = getNode(nodeId);
    featureElement->setUiPosition(newPosition);
}

void babelwires::Project::setNodeContentsSize(NodeId nodeId, const UiSize& newSize) {
    Node* featureElement = getNode(nodeId);
    featureElement->setUiSize(newSize);
}

void babelwires::Project::randomizeProjectId() {
    m_projectId =
        std::uniform_int_distribution<ProjectId>(1, std::numeric_limits<ProjectId>::max())(m_context.m_randomEngine);
}

babelwires::ProjectId babelwires::Project::getProjectId() const {
    return m_projectId;
}

void babelwires::Project::activateOptional(NodeId nodeId, const Path& pathToRecord, ShortId optional,
                                           bool ensureModifier) {
    Node* elementToModify = getNode(nodeId);
    assert(elementToModify);

    ValueTreeNode* const input = elementToModify->getInputNonConst(pathToRecord);
    if (!input) {
        return; // Path cannot be followed.
    }

    ActivateOptionalsModifierData* modifierData = nullptr;

    if (Modifier* existingModifier = elementToModify->getEdits().findModifier(pathToRecord)) {
        if (auto activateOptionalsModifierData =
                existingModifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            auto localModifier = existingModifier->as<LocalModifier>();
            assert(localModifier && "Non-local modifier carrying local data");
            modifierData = activateOptionalsModifierData;
            activateOptionalsModifierData->m_selectedOptionals.emplace_back(optional);
            localModifier->applyIfLocal(m_userLogger, input);
        } else {
            // Discard the existing modifier, since it should be broken anyway.
            assert(existingModifier->isFailed() &&
                   "A non-failed inapplicable modifier was found at an instance of a RecordType");
            removeModifier(nodeId, pathToRecord);
        }
    }

    if (!modifierData) {
        ActivateOptionalsModifierData newData;
        newData.m_targetPath = pathToRecord;
        newData.m_selectedOptionals.emplace_back(optional);
        addModifier(nodeId, newData);
    }
    if (!ensureModifier) {
        removeModifier(nodeId, pathToRecord);
    }
}

void babelwires::Project::deactivateOptional(NodeId nodeId, const Path& pathToRecord, ShortId optional,
                                             bool ensureModifier) {
    Node* elementToModify = getNode(nodeId);
    assert(elementToModify);

    ValueTreeNode* const input = elementToModify->getInputNonConst(pathToRecord);
    if (!input) {
        return; // Path cannot be followed.
    }

    Modifier* existingModifier = elementToModify->getEdits().findModifier(pathToRecord);
    assert(existingModifier);

    auto activateOptionalsModifierData = existingModifier->getModifierData().as<ActivateOptionalsModifierData>();
    assert(activateOptionalsModifierData);
    assert(existingModifier->as<LocalModifier>() && "Non-local modifier carrying local data");
    auto localModifier = static_cast<LocalModifier*>(existingModifier);

    auto it = std::find(activateOptionalsModifierData->m_selectedOptionals.begin(),
                        activateOptionalsModifierData->m_selectedOptionals.end(), optional);
    activateOptionalsModifierData->m_selectedOptionals.erase(it);
    localModifier->applyIfLocal(m_userLogger, input);

    if (!ensureModifier) {
        removeModifier(nodeId, pathToRecord);
    }
}
