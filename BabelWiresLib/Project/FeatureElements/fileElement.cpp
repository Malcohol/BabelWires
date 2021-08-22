/**
 * FileElement are FeatureElements which corresponds to a file (e.g. a source or target file).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/FeatureElements/fileElement.hpp"

#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"

#include <cassert>

babelwires::FileElement::FileElement(const ElementData& data, ElementId newId)
    : FeatureElement(data, newId) {}

bool babelwires::FileElement::reload(const ProjectContext& context, UserLogger& userLogger) {
    assert(!"Reload is not supported on this type of FileElement");
    return false;
}

bool babelwires::FileElement::save(const ProjectContext& context, UserLogger& userLogger) {
    assert(!"Save is not supported on this type of FileElement");
    return false;
}
