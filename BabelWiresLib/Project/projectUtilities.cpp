/**
 * Utilities useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/projectUtilities.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

void babelwires::projectUtilities::translate(const UiPosition& offset, ProjectData& dataInOut) {
    for (auto& elementData : dataInOut.m_elements) {
        elementData->m_uiData.m_uiPosition += offset;
    }
}

namespace {
    void addDerivedValues(
        const babelwires::Project& project, unsigned int valueIndex,
        std::vector<std::tuple<const babelwires::FeatureElement*, babelwires::FeaturePath>>& values) {
        assert(valueIndex < values.size());
        const babelwires::FeatureElement* element = std::get<0>(values[valueIndex]);
        const babelwires::FeaturePath& pathToValue = std::get<1>(values[valueIndex]);
        const babelwires::Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
        const auto& rit = connectionInfo.m_requiredFor.find(element);
        if (rit != connectionInfo.m_requiredFor.end()) {
            for (auto cit : rit->second) {
                const babelwires::ConnectionModifier* connectionModifier = std::get<0>(cit);
                const babelwires::ConnectionModifierData& connectionData = connectionModifier->getModifierData();
                if (connectionData.m_pathToSourceFeature.isPrefixOf(pathToValue)) {
                    // There is a connection between an ancestor features and some feature in the element.
                    babelwires::FeaturePath pathToPossibleArrayInTarget;
                    {
                        babelwires::FeaturePath pathFromAncestor = pathToValue;
                        pathFromAncestor.removePrefix(connectionData.m_pathToSourceFeature.getNumSteps());
                        pathToPossibleArrayInTarget = connectionData.m_pathToFeature;
                        pathToPossibleArrayInTarget.append(pathFromAncestor);
                    }
                    // See if there is a more specific modifier in the target which means this connection does not
                    // actually affect the possible array in the target. Since only connection modifiers can modified
                    // the structure if an array already has a connected ancestor, it should be sufficient to check
                    // connection modifiers.
                    bool foundOverridingModifier = false;
                    const babelwires::FeatureElement* const targetElement = std::get<1>(cit);
                    // TODO The edit tree could provide a O(log N) algorithm for this.
                    for (auto modifier : targetElement->getConnectionModifiers()) {
                        // Using strict here means that connectionModifier itself is exempt from consideration.
                        if (connectionData.m_pathToFeature.isStrictPrefixOf(modifier->getPathToFeature()) &&
                            modifier->getPathToFeature().isPrefixOf(pathToPossibleArrayInTarget)) {
                            foundOverridingModifier = true;
                            break;
                        }
                    }
                    if (!foundOverridingModifier) {
                        assert(pathToPossibleArrayInTarget.tryFollow(*targetElement->getInputFeature()) &&
                               "Expected to find a matching feature in the target, since ancestors are connected and "
                               "there "
                               "are no overriding modifiers");
                        values.emplace_back(std::tuple<const babelwires::FeatureElement*, babelwires::FeaturePath>{
                            targetElement, std::move(pathToPossibleArrayInTarget)});
                    }
                }
            }
        }
    }

} // namespace

std::vector<std::tuple<babelwires::ElementId, babelwires::FeaturePath>>
babelwires::projectUtilities::getAllDerivedValues(const Project& project, ElementId elementId,
                                                  const FeaturePath& pathToValue) {
    std::vector<std::tuple<const FeatureElement*, FeaturePath>> values;
    values.reserve(16);
    unsigned int valueIndex = 0;
    const FeatureElement* element = project.getFeatureElement(elementId);
    values.emplace_back(std::tuple<const FeatureElement*, FeaturePath>{element, pathToValue});
    do {
        addDerivedValues(project, valueIndex, values);
        ++valueIndex;
    } while (valueIndex < values.size());

    std::vector<std::tuple<ElementId, FeaturePath>> convertedValues;
    convertedValues.reserve(values.size());
    // Don't add the first element.
    for (unsigned int i = values.size() - 1; i > 0; --i) {
        convertedValues.emplace_back(std::tuple<ElementId, FeaturePath>{std::get<0>(values[i])->getElementId(),
                                                                        std::move(std::get<1>(values[i]))});
    }
    return convertedValues;
}
