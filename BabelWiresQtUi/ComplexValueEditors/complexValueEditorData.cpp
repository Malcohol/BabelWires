/**
 * A ComplexValueEditorData carries enough data to reconstruct a ComplexValueEditor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include "Common/Utilities/hash.hpp"

std::size_t babelwires::ComplexValueEditorData::getHash() const {
    return hash::mixtureOf(m_elementId, m_pathToValue);
}
