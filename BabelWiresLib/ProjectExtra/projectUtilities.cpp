/**
 * Utilities useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/projectUtilities.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

void babelwires::projectUtilities::translate(const UiPosition& offset, ProjectData& dataInOut) {
    for (auto& elementData : dataInOut.m_elements) {
        elementData->m_uiData.m_uiPosition += offset;
    }
}
