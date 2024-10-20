/**
 * The Project manages the graph of FeatureElements, and propagates data from sources to targets.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/project.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>
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

void babelwires::Project::updateWithAvailableIds(std::vector<ElementId>& idsInOut) const {
    ElementId maxAssignedElementId = m_maxAssignedElementId;
    std::unordered_set<ElementId> nowAssigned;
    for (auto& id : idsInOut) {
        if ((id == INVALID_ELEMENT_ID) || (m_featureElements.find(id) != m_featureElements.end()) ||
            (nowAssigned.find(id) != nowAssigned.end())) {
            ++maxAssignedElementId;
            id = maxAssignedElementId;
        } else {
            maxAssignedElementId = std::max(maxAssignedElementId, id);
        }
        nowAssigned.insert(id);
    }
}

babelwires::ElementId babelwires::Project::reserveElementId(ElementId hint) {
    if ((hint == INVALID_ELEMENT_ID) || (m_featureElements.find(hint) != m_featureElements.end())) {
        ++m_maxAssignedElementId;
        return m_maxAssignedElementId;
    } else {
        m_maxAssignedElementId = std::max(m_maxAssignedElementId, hint);
        return hint;
    }
}

babelwires::FeatureElement* babelwires::Project::addFeatureElementWithoutCachingConnection(const ElementData& data) {
    const ElementId availableId = reserveElementId(data.m_id);
    std::unique_ptr<FeatureElement> elementPtr = data.createFeatureElement(m_context, m_userLogger, availableId);
    FeatureElement* element = elementPtr.get();
    m_featureElements.insert(std::make_pair(availableId, std::move(elementPtr)));
    return element;
}

void babelwires::Project::addFeatureElementConnectionsToCache(FeatureElement* element) {
    for (const auto& connectionModifier : element->getConnectionModifiers()) {
        addConnectionToCache(element, connectionModifier);
    }
}

babelwires::ElementId babelwires::Project::addFeatureElement(const ElementData& data) {
    FeatureElement* element = addFeatureElementWithoutCachingConnection(data);
    addFeatureElementConnectionsToCache(element);
    return element->getElementId();
}

void babelwires::Project::removeElement(ElementId elementId) {
    auto mapIt = m_featureElements.find(elementId);
    assert((mapIt != m_featureElements.end()) && "elementId must refer to an element in the project");
    FeatureElement* element = mapIt->second.get();

    for (const auto& connectionModifier : element->getConnectionModifiers()) {
        removeConnectionFromCache(element, connectionModifier);
    }

    m_removedFeatureElements.insert(std::move(*mapIt));
    m_featureElements.erase(mapIt);
}

void babelwires::Project::addModifier(ElementId elementId, const ModifierData& modifierData) {
    FeatureElement* element = getFeatureElement(elementId);
    assert(element && "Modifier added to unregistered feature element");
    Modifier* modifier = element->addModifier(m_userLogger, modifierData);
    if (ConnectionModifier* connectionModifier = modifier->asConnectionModifier()) {
        addConnectionToCache(element, connectionModifier);
    }
}

void babelwires::Project::removeModifier(ElementId elementId, const FeaturePath& featurePath) {
    FeatureElement* const element = getFeatureElement(elementId);
    assert(element && "Cannot remove a modifier from an element that does not exist");
    Modifier* const modifier = element->findModifier(featurePath);
    assert(modifier && "Cannot remove a modifier that does not exist");
    if (ConnectionModifier* connectionModifier = modifier->asConnectionModifier()) {
        removeConnectionFromCache(element, connectionModifier);
    }
    element->removeModifier(modifier);
}

void babelwires::Project::adjustModifiersInArrayElements(ElementId elementId, const babelwires::FeaturePath& pathToArray,
                                    babelwires::ArrayIndex startIndex, int adjustment) {
    if (adjustment < 0) {
        startIndex -= adjustment;
    }
    FeatureElement* element = getFeatureElement(elementId);
    element->adjustArrayIndices(pathToArray, startIndex, adjustment);

    // Adjust modifiers of other elements affected by the addition.
    // The addition would seem to affect only input features, so this may seem unneccessary.
    // However, we allow input and output features of processors to share paths,
    // in which case, we want any matching output connections to update too.
    const auto r = m_connectionCache.m_requiredFor.find(element);
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

void babelwires::Project::addArrayEntries(ElementId elementId, const FeaturePath& pathToArray, int indexOfNewElement,
                                          int numEntriesToAdd, bool ensureModifier) {
    assert((indexOfNewElement >= 0) && "indexOfNewElement must be positive");
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");

    if (FeatureElement* const element = getFeatureElement(elementId)) {
        if (Feature* const inputFeature = element->getInputFeatureNonConst(pathToArray)) {
            Feature* featureAtPath = pathToArray.tryFollow(*inputFeature);
            assert(featureAtPath && "Path should resolve");
            auto* const arrayFeature = featureAtPath->as<CompoundFeature>();
            assert(arrayFeature && "Path should lead to a compound");
            assert(arrayFeature->as<ValueFeature>() && arrayFeature->as<ValueFeature>()->getType().as<ArrayType>());

            // First, ensure there is an appropriate modifier at the array.
            ArraySizeModifier* arrayModifier = nullptr;
            if (Modifier* const modifier = element->findModifier(pathToArray)) {
                arrayModifier = modifier->as<ArraySizeModifier>();
                if (!arrayModifier && ensureModifier) {
                    // Defensive: Wasn't an array modifier at the path, so let it be replaced.
                    // This won't be restored by an undo, but that shouldn't matter since it isn't
                    // useful.
                    element->removeModifier(modifier);
                }
            }
            if (!arrayModifier) {
                if (ensureModifier) {
                    ArraySizeModifierData arrayInitDataPtr;
                    arrayInitDataPtr.m_pathToFeature = pathToArray;
                    arrayInitDataPtr.m_size = arrayFeature->getNumFeatures();
                    arrayModifier =
                        static_cast<ArraySizeModifier*>(element->addModifier(m_userLogger, arrayInitDataPtr));
                }
            }

            // Next, set the array size.
            if (arrayModifier)
            { 
                arrayModifier->addArrayEntries(m_userLogger, inputFeature, indexOfNewElement, numEntriesToAdd); 
            }

            if (arrayModifier && !ensureModifier) {
                element->removeModifier(arrayModifier);
            }
        }
    }
}

void babelwires::Project::removeArrayEntries(ElementId elementId, const FeaturePath& pathToArray,
                                             int indexOfElementToRemove, int numEntriesToRemove, bool ensureModifier) {
    assert((indexOfElementToRemove >= 0) && "indexOfEntriesToRemove must be positive");
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    if (FeatureElement* const element = getFeatureElement(elementId)) {
        if (Feature* const inputFeature = element->getInputFeatureNonConst(pathToArray)) {
            Feature* featureAtPath = pathToArray.tryFollow(*inputFeature);
            assert(featureAtPath && "Path should resolve");
            auto* const arrayFeature = featureAtPath->as<CompoundFeature>();
            assert(arrayFeature && "Path should lead to a compound");
            assert(arrayFeature->as<ValueFeature>() && arrayFeature->as<ValueFeature>()->getType().as<ArrayType>());

            // First, check if there is a modifier at the array.
            ArraySizeModifier* arrayModifier = nullptr;
            if (Modifier* const modifier = element->findModifier(pathToArray)) {
                arrayModifier = modifier->as<ArraySizeModifier>();
                if (!arrayModifier && ensureModifier) {
                    // Defensive: Wasn't an array modifier at the path, so let it be replaced.
                    // This won't be restored by an undo, but that shouldn't matter since it isn't
                    // useful.
                    element->removeModifier(modifier);
                }
            }
            if (!arrayModifier) {
                if (ensureModifier) {
                    ArraySizeModifierData arrayInitDataPtr;
                    arrayInitDataPtr.m_pathToFeature = pathToArray;
                    arrayInitDataPtr.m_size = arrayFeature->getNumFeatures();
                    arrayModifier =
                        static_cast<ArraySizeModifier*>(element->addModifier(m_userLogger, arrayInitDataPtr));
                }
            }

            // Next, set the array size.
            if (arrayModifier) {
                arrayModifier->removeArrayEntries(m_userLogger, inputFeature, indexOfElementToRemove,
                                                  numEntriesToRemove);
            }

            if (arrayModifier && !ensureModifier) {
                element->removeModifier(arrayModifier);
            }
        }
    }
}

void babelwires::Project::setProjectData(const ProjectData& projectData) {
    clear();

    if (projectData.m_projectId != INVALID_PROJECT_ID) {
        m_projectId = projectData.m_projectId;
    }
    std::vector<FeatureElement*> elementsAdded;
    for (const auto& elementData : projectData.m_elements) {
        elementsAdded.emplace_back(addFeatureElementWithoutCachingConnection(*elementData));
    }
    for (auto* element : elementsAdded) {
        addFeatureElementConnectionsToCache(element);
    }
}

babelwires::ProjectData babelwires::Project::extractProjectData() const {
    ProjectData projectData;
    projectData.m_projectId = m_projectId;
    for (const auto& pair : m_featureElements) {
        projectData.m_elements.emplace_back(pair.second->extractElementData());
    }
    return projectData;
}

void babelwires::Project::clear() {
    // TODO Why not just clear? This isn't undoable.
    m_removedFeatureElements.swap(m_featureElements);
    m_featureElements.clear();
    setConnectionCacheInvalid();
    randomizeProjectId();
    m_maxAssignedElementId = 0;
}

babelwires::Project::~Project() {}

const babelwires::TargetFileFormatRegistry& babelwires::Project::getFactoryFormatRegistry() const {
    return m_context.m_targetFileFormatReg;
}

const babelwires::SourceFileFormatRegistry& babelwires::Project::getFileFormatRegistry() const {
    return m_context.m_sourceFileFormatReg;
}

babelwires::FeatureElement* babelwires::Project::getFeatureElement(ElementId id) {
    auto&& it = m_featureElements.find(id);
    if (it != m_featureElements.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

const babelwires::FeatureElement* babelwires::Project::getFeatureElement(ElementId id) const {
    auto&& it = m_featureElements.find(id);
    if (it != m_featureElements.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

void babelwires::Project::tryToReloadAllSources() {
    int attemptedReloads = 0;
    int successfulReloads = 0;
    for (const auto& [_, f] : m_featureElements) {
        if (FileElement* const fileElement = f->as<FileElement>()) {
            if (isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
                ++attemptedReloads;
                if (fileElement->reload(m_context, m_userLogger)) {
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
    for (const auto& [_, f] : m_featureElements) {
        if (FileElement* const fileElement = f->as<FileElement>()) {
            if (isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::save)) {
                ++attemptedSaves;
                if (fileElement->save(m_context, m_userLogger)) {
                    ++successfulSaves;
                }
            }
        }
    }
    m_userLogger.logInfo() << "Saved " << successfulSaves << "/" << attemptedSaves << " files.";
}

void babelwires::Project::tryToReloadSource(ElementId id) {
    assert((id != INVALID_ELEMENT_ID) && "Invalid id");
    FeatureElement* f = getFeatureElement(id);
    assert(f && "There was no such feature element");
    FileElement* const fileElement = f->as<FileElement>();
    assert(fileElement && "There was no such file element");
    assert(isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload) &&
           "There was no such reloadable file element");
    fileElement->reload(m_context, m_userLogger);
}

void babelwires::Project::tryToSaveTarget(ElementId id) {
    assert((id != INVALID_ELEMENT_ID) && "Invalid id");
    FeatureElement* f = getFeatureElement(id);
    assert(f && "There was no such feature element");
    FileElement* const fileElement = f->as<FileElement>();
    assert(fileElement && "There was no such file element");
    assert(isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::save) &&
           "There was no such saveable file element");
    fileElement->save(m_context, m_userLogger);
}

void babelwires::Project::setConnectionCacheInvalid() {
    m_connectionCache.m_dependsOn.clear();
    m_connectionCache.m_requiredFor.clear();
    m_connectionCache.m_brokenConnections.clear();
}

void babelwires::Project::addConnectionToCache(FeatureElement* element, ConnectionModifier* connectionModifier) {
    if (auto source = getFeatureElement(connectionModifier->getModifierData().m_sourceId)) {
        {
            auto itAndBool = m_connectionCache.m_dependsOn.insert(
                std::make_pair(element, babelwires::Project::ConnectionInfo::Connections()));
            babelwires::Project::ConnectionInfo::Connections& connections = itAndBool.first->second;
            connections.emplace_back(std::make_tuple(connectionModifier, source));
        }
        {
            auto itAndBool = m_connectionCache.m_requiredFor.insert(
                std::make_pair(source, babelwires::Project::ConnectionInfo::Connections()));
            babelwires::Project::ConnectionInfo::Connections& connections = itAndBool.first->second;
            connections.emplace_back(std::make_tuple(connectionModifier, element));
        }
    } else {
        m_connectionCache.m_brokenConnections.emplace_back(std::make_tuple(connectionModifier, element));
    }
}

void babelwires::Project::removeConnectionFromCache(FeatureElement* element, ConnectionModifier* connectionModifier) {
    if (auto source = getFeatureElement(connectionModifier->getModifierData().m_sourceId)) {
        {
            auto dit = m_connectionCache.m_dependsOn.find(element);
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
            auto cit = std::find(connections.begin(), connections.end(), std::make_tuple(connectionModifier, element));
            assert((cit != connections.end()) && "Cannot find connection to remove");
            connections.erase(cit);
            if (connections.empty()) {
                m_connectionCache.m_requiredFor.erase(rit);
            }
        }
    } else {
        auto it = std::find(m_connectionCache.m_brokenConnections.begin(), m_connectionCache.m_brokenConnections.end(),
                            std::make_tuple(connectionModifier, element));
        m_connectionCache.m_brokenConnections.erase(it);
    }
}

void babelwires::Project::validateConnectionCache() const {
#ifndef NDEBUG
    const auto checkOwned = [this](const FeatureElement* pointerToCheck) {
        const auto it =
            std::find_if(m_featureElements.begin(), m_featureElements.end(),
                         [pointerToCheck](const auto& uPtr) { return std::get<1>(uPtr).get() == pointerToCheck; });
        assert((it != m_featureElements.end()) && "The cache refers to an element that is not owned by the project");
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

    for (const auto& pair : m_featureElements) {
        const FeatureElement* target = pair.second.get();

        for (const auto& connectionModifier : target->getConnectionModifiers()) {
            if (auto source = getFeatureElement(connectionModifier->getModifierData().m_sourceId)) {
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

void babelwires::Project::propagateChanges(const FeatureElement* e) {
    const auto r = m_connectionCache.m_requiredFor.find(e);
    if (r != m_connectionCache.m_requiredFor.end()) {
        const ConnectionInfo::Connections& connections = r->second;
        for (auto&& pair : connections) {
            ConnectionModifier* connection = std::get<0>(pair);
            FeatureElement* targetElement = std::get<1>(pair);
            if (Feature* inputFeature = targetElement->getInputFeatureNonConst(connection->getPathToFeature())) {
                connection->applyConnection(*this, m_userLogger, inputFeature);
            }
        }
    }

    // Check that all broken connections are marked failed.
    for (auto pair : m_connectionCache.m_brokenConnections) {
        ConnectionModifier* connection = std::get<0>(pair);
        FeatureElement* owner = std::get<1>(pair);
        if (!connection->isFailed()) {
            if (Feature* inputFeature = owner->getInputFeatureNonConst(connection->getPathToFeature())) {
                connection->applyConnection(*this, m_userLogger, inputFeature);
            }
        }
    }
}

void babelwires::Project::process() {
    validateConnectionCache();

    // Topologically sort the featureElements in a later-depends-on-earlier order.
    std::vector<FeatureElement*> sortedElements;

    sortedElements.reserve(m_featureElements.size());
    for (auto&& pair : m_featureElements) {
        sortedElements.emplace_back(pair.second.get());
    }

    std::unordered_map<const FeatureElement*, int> numDependencies;
    numDependencies.reserve(m_connectionCache.m_dependsOn.size());
    for (auto&& pair : m_connectionCache.m_dependsOn) {
        numDependencies.insert(std::make_pair(pair.first, pair.second.size()));
    }

    // Because we're marking the elements as they are sorted, we have to iterate to the end.
    for (int firstUnsortedIndex = 0; firstUnsortedIndex < m_featureElements.size();) {
        const int firstUnsortedIndexBefore = firstUnsortedIndex;
        for (int j = firstUnsortedIndex; j < m_featureElements.size(); ++j) {
            FeatureElement* element = sortedElements[j];

            const auto it = numDependencies.find(element);
            if (it == numDependencies.end() || it->second == 0) {
                std::swap(sortedElements[firstUnsortedIndex], sortedElements[j]);
                element->setInDependencyLoop(false);
                ++firstUnsortedIndex;
                const auto r = m_connectionCache.m_requiredFor.find(element);
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
            for (int i = firstUnsortedIndex; i < m_featureElements.size(); ++i) {
                sortedElements[i]->setInDependencyLoop(true);
            }
            break;
        }
    }

    // Now iterate in dependency order.
    for (auto&& element : sortedElements) {
        element->process(*this, m_userLogger);
        // Existing connections only apply their contents if their source has changed,
        // so this doesn't unnecessarily change dependent data.
        // We do need to visit all out-going connections in case some are new.
        propagateChanges(element);
    }
}

const std::map<babelwires::ElementId, std::unique_ptr<babelwires::FeatureElement>>&
babelwires::Project::getElements() const {
    return m_featureElements;
}

void babelwires::Project::clearChanges() {
    for (auto&& pair : m_featureElements) {
        pair.second->clearChanges();
    }
    m_removedFeatureElements.clear();
}

const std::map<babelwires::ElementId, std::unique_ptr<babelwires::FeatureElement>>&
babelwires::Project::getRemovedElements() const {
    return m_removedFeatureElements;
}

void babelwires::Project::setElementPosition(ElementId elementId, const UiPosition& newPosition) {
    FeatureElement* featureElement = getFeatureElement(elementId);
    featureElement->setUiPosition(newPosition);
}

void babelwires::Project::setElementContentsSize(ElementId elementId, const UiSize& newSize) {
    FeatureElement* featureElement = getFeatureElement(elementId);
    featureElement->setUiSize(newSize);
}

void babelwires::Project::randomizeProjectId() {
    m_projectId =
        std::uniform_int_distribution<ProjectId>(1, std::numeric_limits<ProjectId>::max())(m_context.m_randomEngine);
}

babelwires::ProjectId babelwires::Project::getProjectId() const {
    return m_projectId;
}

void babelwires::Project::activateOptional(ElementId elementId, const FeaturePath& pathToRecord, ShortId optional,
                                           bool ensureModifier) {
    FeatureElement* elementToModify = getFeatureElement(elementId);
    assert(elementToModify);

    Feature* const inputFeature = elementToModify->getInputFeatureNonConst(pathToRecord);
    if (!inputFeature) {
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
            localModifier->applyIfLocal(m_userLogger, inputFeature);
        } else {
            // Discard the existing modifier, since it should be broken anyway.
            assert(existingModifier->isFailed() &&
                   "A non-failed inapplicable modifier was found at an instance of a RecordType");
            removeModifier(elementId, pathToRecord);
        }
    }

    if (!modifierData) {
        ActivateOptionalsModifierData newData;
        newData.m_pathToFeature = pathToRecord;
        newData.m_selectedOptionals.emplace_back(optional);
        addModifier(elementId, newData);
    }
    if (!ensureModifier) {
        removeModifier(elementId, pathToRecord);
    }
}

void babelwires::Project::deactivateOptional(ElementId elementId, const FeaturePath& pathToRecord, ShortId optional,
                                             bool ensureModifier) {
    FeatureElement* elementToModify = getFeatureElement(elementId);
    assert(elementToModify);

    Feature* const inputFeature = elementToModify->getInputFeatureNonConst(pathToRecord);
    if (!inputFeature) {
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
    localModifier->applyIfLocal(m_userLogger, inputFeature);

    if (!ensureModifier) {
        removeModifier(elementId, pathToRecord);
    }
}
